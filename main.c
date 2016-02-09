#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "util.h"

typedef struct {
    bool finished;

    uint16_t total;

    char frame_result[10][4];
    uint16_t frame_score[10];
} game_state_s;

define_err_s(game_state_s, game_state);

enum {
    FIRST,
    SECOND
};

define_err_s(uint8_t, throw_state);

static const char err_not_valid_char[] = "Not valid game description character";
static const char err_bad_args[] = "Not valid game description character";
static const char err_stike_on_first_only[] = "Strike can only be on first throw in frame";
static const char err_spare_on_second_only[] = "Spare can only be on second throw in frame";
static const char err_too_many_throws[] = "To many throws";

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

    state = FIRST;
    uint8_t frame = 0;
    for (uint8_t throw = 0; throw < total_throws; throw++) {
        char gc = game_description[throw];

        bool last_frame_finished = false;
            
        // Errors checked in first loop.
        throw_state_err_s next_state = next_throw_state(state, gc);

        ret.value.frame_result[frame][state] = throw_sym[throw];
        ret.value.frame_score[frame] += throw_score[throw];

        if (is_strike(gc)) {
            ret.value.frame_score[frame] += throw_score[throw + 1];
            ret.value.frame_score[frame] += throw_score[throw + 2];
        } else if (is_spare(gc)) {
            ret.value.frame_score[frame] += throw_score[throw + 1];
        }

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

    for (uint8_t frame = 0; frame < 10; frame++) {
        ret.value.total += ret.value.frame_score[frame];
    }

fail:
    return ret;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Error: %s\n", err_bad_args);
        return 1;
    }

    game_state_err_s state = calculate_game_state(argv[1]);

    if (state.error) {
        printf("Error: %s\n", state.error);
        return 1;
    }

    printf("Finished: %s\n", state.value.finished ? "yes" : "no");
    for (int i = 0; i < 10; ++i) {
        printf("%2d | %3s: %d\n",
               i + 1,
               state.value.frame_result[i],
               state.value.frame_score[i]);
    }
    printf("Total: %d\n", state.value.total);

    return 0;
}

