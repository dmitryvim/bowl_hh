#ifndef __ATTOTEST_H__
#define __ATTOTEST_H__

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char *_name;
    int (*_run)();
} attotest_s;

#define PASS 1
#define FAIL 0

#define TEST(name) \
    int attotest_##name##_run(); \
    attotest_s attotest_##name __attribute__((section("ATTOTEST_TESTS"))) = { \
        ._name = #name, \
        ._run = attotest_##name##_run \
    }; \
    int attotest_##name##_run()

#define RUN_TESTS()\
    int main() \
    { \
        int pass = 0; \
        int fail = 0; \
     \
        extern attotest_s __start_ATTOTEST_TESTS; \
        extern attotest_s __stop_ATTOTEST_TESTS; \
     \
        for (attotest_s *test = &__start_ATTOTEST_TESTS; \
                test < &__stop_ATTOTEST_TESTS; \
                test++) { \
            int res = (*test)._run(); \
            printf("%s: %s\n", res? "PASS" : "FAIL", (*test)._name); \
            if (res == PASS) \
                pass++; \
            else \
                fail++; \
        } \
        printf("\n"); \
        if (!fail) \
            printf("ALL PASS\n"); \
        else \
            printf("PASS: %d  FAIL: %d\n", pass, fail); \
        return (fail != 0); \
    }

#define atto_assert(test) do \
    { \
        if (!(test)) \
        { \
            printf("%s:%d:0: warning: assertion %s failed\n", __FILE__, __LINE__, #test); \
            return FAIL; \
        } \
    } while (0)

#endif //__ATTOTEST_H__
