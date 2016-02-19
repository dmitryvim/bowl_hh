#include "attotest.h"

#include "bowl.h"

game_state_err_s
calculate_game_state(const char* game_description);

TEST(perfect_game)
{
    game_state_err_s state = calculate_game_state("xxxxxxxxxxxx");

    atto_assert(state.error == 0);
    atto_assert(state.value.finished == true);
    atto_assert(state.value.total == 300);

    return PASS;
}

TEST(zero_game)
{
    game_state_err_s state = calculate_game_state("00000000000000000000");

    atto_assert(state.error == 0);
    atto_assert(state.value.finished == true);
    atto_assert(state.value.total == 0);

    return PASS;
}

TEST(one_throw)
{
    game_state_err_s state = {0};
    game_state_err_s next_state =
        next_game_state(&state.value, '4');
    atto_assert(next_state.error == 0);
    atto_assert(next_state.value.current_throw == 1);
    atto_assert(next_state.value.throw_score[0] == 4);
    atto_assert(next_state.value.frame_score[0] == 4);
    atto_assert(next_state.value.total == 4);
    
    return PASS;
}

TEST(two_throws)
{
    game_state_err_s state = {0};

    game_state_err_s next_state =
        next_game_state(&state.value, '4');
    atto_assert(next_state.error == 0);

    next_state =
        next_game_state(&next_state.value, '4');
    atto_assert(next_state.error == 0);

    atto_assert(next_state.value.finished == false);
    atto_assert(next_state.value.current_throw == 2);
    atto_assert(next_state.value.throw_score[0] == 4);
    atto_assert(next_state.value.throw_score[1] == 4);
    atto_assert(next_state.value.frame_score[0] == 8);
    atto_assert(next_state.value.total == 8);
    
    return PASS;
}

TEST(xxx_throw)
{
    game_state_err_s state = {0};

    game_state_err_s next_state =
        next_game_state(&state.value, 'x');
    atto_assert(next_state.error == 0);

    next_state =
        next_game_state(&next_state.value, 'x');
    atto_assert(next_state.error == 0);

    next_state =
        next_game_state(&next_state.value, 'x');
    atto_assert(next_state.error == 0);

    atto_assert(next_state.value.current_throw == 6);

    atto_assert(next_state.value.throw_score[0] == 10);
    atto_assert(next_state.value.throw_score[1] == 0);
    atto_assert(next_state.value.throw_score[2] == 10);
    atto_assert(next_state.value.throw_score[3] == 0);
    atto_assert(next_state.value.throw_score[4] == 10);
    atto_assert(next_state.value.throw_score[5] == 0);

    atto_assert(next_state.value.frame_score[0] == 30);
    atto_assert(next_state.value.frame_score[1] == 20);
    atto_assert(next_state.value.frame_score[2] == 10);

    atto_assert(next_state.value.total == 60);

    return PASS;
}

TEST(spare)
{
    game_state_err_s state = {0};

    game_state_err_s next_state =
        next_game_state(&state.value, '8');
    atto_assert(next_state.error == 0);

    next_state =
        next_game_state(&next_state.value, '/');
    atto_assert(next_state.error == 0);

    next_state =
        next_game_state(&next_state.value, '8');
    atto_assert(next_state.error == 0);

    next_state =
        next_game_state(&next_state.value, '2');
    atto_assert(next_state.error == 0);

    atto_assert(next_state.value.current_throw == 4);

    atto_assert(next_state.value.throw_score[0] == 8);
    atto_assert(next_state.value.throw_score[1] == 2);
    atto_assert(next_state.value.throw_score[2] == 8);
    atto_assert(next_state.value.throw_score[3] == 2);

    atto_assert(next_state.value.frame_score[0] == 18);
    atto_assert(next_state.value.frame_score[1] == 10);

    atto_assert(next_state.value.total == 28);

    return PASS;
}

RUN_TESTS()
