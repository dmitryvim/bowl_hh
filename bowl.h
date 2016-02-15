#ifndef __BOWL_H__
#     define __BOWL_H__

#include <stdbool.h>
#include <stdint.h>

#include "util.h"

static const char err_not_valid_char[] = "Not valid game description character";
static const char err_stike_on_first_only[] = "Strike can only be on first throw in frame";
static const char err_spare_on_second_only[] = "Spare can only be on second throw in frame";
static const char err_too_many_throws[] = "To many throws";

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

game_state_err_s
calculate_game_state(const char* game_description);

#endif /* __BOWL_H__ */
