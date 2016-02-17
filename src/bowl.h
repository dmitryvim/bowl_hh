#ifndef __BOWL_H__
#     define __BOWL_H__

#include <stdbool.h>
#include <stdint.h>

#include "util.h"

// Errors list
static const char err_not_valid_char[] = "Not valid game description character";
static const char err_stike_on_first_only[] = "Strike can only be on first throw in frame";
static const char err_spare_on_second_only[] = "Spare can only be on second throw in frame";
static const char err_too_many_throws[] = "To many throws";

// This structure holds full game state
typedef struct {
    bool finished;

    uint16_t total;

    char frame_result[10][4];
    uint16_t frame_score[10];
} game_state_s;

// Wrapper which contains game_state_s or error.
// It's like Optional<> in Java.
// See util.h
define_err_s(game_state_s, game_state);

// Calc game state
// game_description is a string, containing:
//   - digits for regular throws
//   - '/' for spare;
//   - 'x' or 'X' for strike;
//   - 'f' or 'F' for faults.
// no spaces or other charackters allowed.
game_state_err_s
calculate_game_state(const char* game_description);

#endif /* __BOWL_H__ */
