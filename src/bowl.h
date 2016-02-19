#ifndef __BOWL_H__
#     define __BOWL_H__

#include <stdbool.h>
#include <stdint.h>

#include "util.h"

// Errors list
static const char err_game_is_finished[] = "Game is finished";
static const char err_more_than_ten_pins[] = "There's only 10 pins";
static const char err_not_valid_char[] = "Not valid game description character";
static const char err_stike_on_first_only[] = "Strike can only be on first throw in frame";
static const char err_spare_on_second_only[] = "Spare can only be on second throw in frame";
static const char err_too_many_throws[] = "To many throws";

// Magic numbers
#define MAX_THROWS 23 //TODO: shoud be 21. Fix last round handling
#define MAX_FRAMES 10
#define TOTAL_PINS 10

// This structure holds full game state
typedef struct {
    bool finished;

    uint16_t total;

    uint8_t current_throw;
    uint8_t throw_state;

    uint8_t throw_score[MAX_THROWS];
    char throw_char[MAX_THROWS];

    uint16_t frame_score[MAX_FRAMES];
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

// Given current game state and next throw returns new game state
// valid throw is on of
//   - digit for regular throws
//   - '/' for spare;
//   - 'x' or 'X' for strike;
//   - 'f' or 'F' for faults.
game_state_err_s
next_game_state(const game_state_s *game_state, char throw_char);

#endif /* __BOWL_H__ */
