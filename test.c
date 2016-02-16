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

TEST(one_throw)
{
    game_state_err_s state = calculate_game_state("1");

    atto_assert(state.error == 0);
    atto_assert(state.value.finished == false);
    atto_assert(state.value.total == 1);

    return PASS;
}

TEST(second_throw)
{
    game_state_err_s state = calculate_game_state("44");

    atto_assert(state.error == 0);
    atto_assert(state.value.finished == false);
    atto_assert(state.value.total == 8);

    return PASS;
}

RUN_TESTS()
