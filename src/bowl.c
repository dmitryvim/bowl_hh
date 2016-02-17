#include "bowl.h"

#include <string.h>

// Possible frame states
enum {
    FIRST,
    SECOND
};

// Wrapper which contains throw_state or error.
// It's like Optional<> in Java.
// See util.h
define_err_s(uint8_t, throw_state);

bool
is_digit(char gc)
{
    if (gc >= '0' && gc <= '9')
        return true;

    return false;
}

bool
is_strike(char gc)
{
    if (gc == 'x' || gc == 'X')
        return true;

    return false;
}

bool
is_spare(char gc)
{
    if (gc == '/')
        return true;

    return false;
}

bool
is_fault(char gc)
{
    if (gc == 'f' || gc == 'F')
        return true;

    return false;
}

bool
is_zero(char gc)
{
    if (gc == '-' || gc == '0')
        return true;

    return false;
}

// Given current state and next throw char
// returns next frame state.
throw_state_err_s
next_throw_state(uint8_t state, char gc)
{
    throw_state_err_s ret = {0};

    if (is_digit(gc) || is_zero(gc) || is_fault(gc)) {
        ret.value = (state == FIRST) ? SECOND : FIRST;
    } else if (is_strike(gc)) {
        if (state == SECOND) {
            ret.error = err_stike_on_first_only;
        } else {
            ret.value = FIRST;
        }
    } else if (is_spare(gc)) {
        if (state == FIRST) {
            ret.error = err_spare_on_second_only;
        } else {
            ret.value = FIRST;
        }
    } else {
        ret.error = err_not_valid_char;
    }

    return ret;
}

game_state_err_s
calculate_game_state(const char* game_description)
{
    // This function calculates game score.
    // This is done it three steps:
    //   - Calculate points per throw.
    //   - Calculate points per frame (this handles strike and spare).
    //   - Calculate total score.

    game_state_err_s ret = {0};

    size_t total_throws = strlen(game_description);

    // Add two fake throws in case of last throw is strike.
    uint8_t const max_bonus_throws = 2;
    uint8_t throw_score[total_throws + max_bonus_throws];
    char throw_sym[total_throws + max_bonus_throws];
    memset(throw_score, 0, sizeof(throw_score) * sizeof(uint8_t));
    memset(throw_sym, 0, sizeof(throw_sym));

    uint8_t state = FIRST;

    // Get points per throw
    for (uint8_t throw = 0; throw < total_throws; throw++) {
        char gc = game_description[throw];

        throw_state_err_s next_state = next_throw_state(state, gc);
        if (next_state.error) {
            ret.error = next_state.error;
            goto fail;
        }

        if (is_zero(gc)) {
            throw_score[throw] = 0;
            throw_sym[throw] = '-';
        } else if (is_digit(gc)) {
            throw_score[throw] = gc - '0';
            throw_sym[throw] = gc;
        } else if (is_strike(gc)) {
            throw_score[throw] = 10;
            throw_sym[throw] = 'X';
        } else if (is_spare(gc)) {
            throw_score[throw] = 10 - throw_score[throw - 1];
            throw_sym[throw] = '/';
        } else if (is_fault(gc)) {
            throw_score[throw] = 0;
            throw_sym[throw] = 'F';
        } else {
            ret.error = err_not_valid_char;
            goto fail;
        }

        state = next_state.value;
    }

    // Get points per frame
    state = FIRST;
    uint8_t frame = 0;
    for (uint8_t throw = 0; throw < total_throws; throw++) {
        char gc = game_description[throw];

        bool last_frame_finished = false;

        // Errors checked in first loop.
        throw_state_err_s next_state = next_throw_state(state, gc);

        ret.value.frame_result[frame][state] = throw_sym[throw];
        ret.value.frame_score[frame] += throw_score[throw];

        // Special logic for strike an spare
        if (is_strike(gc)) {
            ret.value.frame_score[frame] += throw_score[throw + 1];
            ret.value.frame_score[frame] += throw_score[throw + 2];
        } else if (is_spare(gc)) {
            ret.value.frame_score[frame] += throw_score[throw + 1];
        }

        // Last frames checks
        if (frame == 9) {
            if (is_strike(gc)) {
                ret.value.frame_result[frame][state + 1] = throw_sym[++throw];
                ret.value.frame_result[frame][state + 2] = throw_sym[++throw];
                last_frame_finished = true;
            } else if (is_spare(gc)) {
                ret.value.frame_result[frame][state + 1] = throw_sym[++throw];
                last_frame_finished = true;
            } else if (state == SECOND) {
                last_frame_finished = true;
            }
        }

        state = next_state.value;

        if (state == FIRST) {
            frame++;
        }

        if (last_frame_finished) {
            ret.value.finished = true;

            if (total_throws - throw > 1) {
                ret.error = err_too_many_throws;
                goto fail;
            }

            break;
        }
    }

    // calculate total scores
    for (uint8_t frame = 0; frame < 10; frame++) {
        ret.value.total += ret.value.frame_score[frame];
    }

fail:
    return ret;
}


