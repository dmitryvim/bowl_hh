#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "util.h"

#include "bowl.h"

static const char err_bad_args[] = "Not valid game description character";

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
        printf("%2d | %2.2s %d\n",
               i + 1,
               &state.value.throw_char[i * 2],
               state.value.frame_score[i]);

    }
    printf("Total: %d\n", state.value.total);

    return 0;
}

