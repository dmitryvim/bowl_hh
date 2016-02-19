#include "bowl.h"

#include <assert.h>
#include <string.h>

// Possible frame states
enum {
    FIRST,
    SECOND,
};

define_err_s(uint8_t, throw_score);
define_err_s(char, throw_char);
define_err_s(uint8_t, bonus);

static bool
is_digit(char gc)
{
    if (gc >= '0' && gc <= '9')
        return true;

    return false;
}

static bool
is_strike(char gc)
{
    if (gc == 'x' || gc == 'X')
        return true;

    return false;
}

static bool
is_spare(char gc)
{
    if (gc == '/')
        return true;

    return false;
}

static bool
is_fault(char gc)
{
    if (gc == 'f' || gc == 'F')
        return true;

    return false;
}

static bool
is_zero(char gc)
{
    if (gc == '-' || gc == '0')
        return true;

    return false;
}

static inline uint8_t
prev_throw_score(const game_state_s *const gs)
{
    assert(gs);
    assert(gs->current_throw > 0);

    return gs->throw_score[gs->current_throw - 1];
}

static inline uint8_t
is_second_in_frame(const game_state_s *const gs)
{
    assert(gs);

    return (gs->current_throw % 2 == 1);
}

static throw_score_err_s
next_throw_score(const game_state_s *const gs, char throw_char)
{
    assert(gs);

    throw_score_err_s ret = {0};

    if (is_digit(throw_char)) {
        ret.value = throw_char - '0';
        if (is_second_in_frame(gs)) {
            if (ret.value + prev_throw_score(gs) > TOTAL_PINS) {
                ret.error = err_more_than_ten_pins;
                goto fail;
            }
        }
    } else if (is_spare(throw_char)) {
        if (!is_second_in_frame(gs)) {
            ret.error = err_spare_on_second_only;
            goto fail;
        }
        ret.value = TOTAL_PINS - prev_throw_score(gs);
    } else if (is_strike(throw_char)) {
        if (is_second_in_frame(gs)) {
            ret.error = err_stike_on_first_only;
            goto fail;
        }
        ret.value = TOTAL_PINS;
    }

fail:
    return ret;
}

static throw_char_err_s
next_throw_char(const game_state_s *const gs,
                char throw_char,
                uint8_t throw_score)
{
    assert(gs);

    throw_char_err_s ret = {0};

    if (is_zero(throw_char)) {
        ret.value = '-';
    } else if (is_digit(throw_char)) {
        ret.value = throw_char;
        if (is_second_in_frame(gs)
            && throw_score + prev_throw_score(gs) == TOTAL_PINS) {
            ret.value = '/';
        }
    } else if (is_spare(throw_char)) {
        ret.value = '/';
    } else if (is_strike(throw_char)) {
        ret.value = 'X';
    } else if (is_fault(throw_char)) {
        ret.value = 'F';
    }

fail:
    return ret;
}

static uint8_t
strike_bonus(const game_state_s *const gs, uint8_t throw)
{
    assert(gs);
    uint8_t ret = 0;

    // No strike - no bonus
    if (gs->throw_score[throw] != 10) {
        return 0;
    }

    // No bonus for last frame bonus throws
    if (throw > 18) {
        return 0;
    }

    ret += gs->throw_score[throw + 2];

    if (ret == 10) {
        // Two srikes in a row
        ret += gs->throw_score[throw + 4];
    } else {
        ret += gs->throw_score[throw + 3];
    }

    return ret;
}

static uint8_t
spare_bonus(const game_state_s *const gs, uint8_t throw)
{
    assert(gs);
    uint8_t ret = 0;

    // No spare - no bonus
    if (gs->throw_score[throw] + gs->throw_score[throw - 1] != 10) {
        return 0;
    }

    // No bonus for lat frame bonus throws
    if (throw > 19) {
        return 0;
    }

    ret += gs->throw_score[throw + 1];

    return ret;
}

static bool
is_game_finished(const game_state_s *const gs)
{
    assert(gs);

    if (gs->current_throw > MAX_THROWS) {
                return true;
    }

    if (gs->current_throw > 19) {
        if (gs->frame_score[9] < 10) {
                return true;
        }
    }

    return false;
}

game_state_err_s
next_game_state(const game_state_s *const game_state, char throw_char)
{
    assert(game_state);
    game_state_err_s ret = {0};

    if (game_state->finished) {
        ret.error = err_game_is_finished;
        goto fail;
    }

    ret.value = *game_state;
    // Just shortcut
    game_state_s *gs = &ret.value;

    throw_score_err_s score = next_throw_score(game_state, throw_char);
    if (score.error) {
        ret.error = score.error;
        goto fail;
    }
    gs->throw_score[gs->current_throw] = score.value;

    throw_char_err_s tchar =
        next_throw_char(game_state, throw_char, score.value);
    if (tchar.error) {
        ret.error = tchar.error;
        goto fail;
    }
    gs->throw_char[gs->current_throw] = tchar.value;

    // This can be optimized but it's only 10 iterations
    gs->total = 0;
    for (int frame = 0; frame < MAX_FRAMES; frame++) {
        gs->frame_score[frame] = gs->throw_score[frame * 2];
        gs->frame_score[frame] += gs->throw_score[frame * 2 + 1];

        if (gs->throw_score[frame * 2] == 10) {
            gs->frame_score[frame] += strike_bonus(gs, frame * 2);
        } else if (gs->throw_score[frame * 2]
                   + gs->throw_score[frame * 2 + 1] == 10) {
            gs->frame_score[frame] += spare_bonus(gs, frame * 2 + 1);
        }


        gs->total += gs->frame_score[frame];
    }

    // throw increment
    gs->current_throw++;
    if (is_strike(throw_char)) {
        gs->current_throw++;
    }

    gs->finished = is_game_finished(gs);
fail:
    return ret;
}

game_state_err_s
calculate_game_state(const char* game_description)
{
    game_state_err_s ret = {0};

    for (int throw = 0; throw < strlen(game_description); throw++)
    {
        ret = next_game_state(&ret.value, game_description[throw]);
        if (ret.error) {
            goto fail;
        }
    }

fail:
    return ret;
}

