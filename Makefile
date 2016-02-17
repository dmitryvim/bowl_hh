PROG = bowl

FLAVOR ?= debug

CFLAGS = -std=gnu99
CFLAGS += -Wall -Wextra
CFLAGS += -MMD

debug_CFLAGS = -g
coverage_CFLAGS = -g -fprofile-arcs -ftest-coverage 

CFLAGS += $($(FLAVOR)_CFLAGS)

SRC_DIR = src
TESTS_DIR = tests
OBJ_DIR = $(FLAVOR)

MAIN_SRC = $(SRC_DIR)/main.c
TEST_SRC = $(SRC_DIR)/test.c

ALL_SRC = $(wildcard $(SRC_DIR)/*.c)
PROG_SRC = $(filter-out $(MAIN_SRC) $(TEST_SRC),$(ALL_SRC))
PROG_OBJ = $(PROG_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
PROG_DEP = $(PROG_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.d)
MAIN_OBJ = $(OBJ_DIR)/main.o
TEST_OBJ = $(OBJ_DIR)/test.o

all: prog test
	
prog: $(PROG)-$(FLAVOR)
test: test-$(FLAVOR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(PROG)-$(FLAVOR): $(PROG_OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $+ $(LIBS)

test-$(FLAVOR): $(PROG_OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $+ $(LIBS)

run-test: test-$(FLAVOR)
	./test-$(FLAVOR) | tee test.log

ifeq ($(FLAVOR), coverage)
gen-test-coverage: run-test
	lcov --capture --directory coverage --output-file coverage.info
	genhtml coverage.info --output-directory coverage_html
else
gen-test-coverage:
	$(error gen-test-coverage target only avalable when FLAVOR=coverage)
endif

clean:
	-rm -rf ./$(PROG)-* \
		./test-* \
		./debug ./release ./coverage \
		./coverage_html coverage.info \
		test.log

-include $(PROG_DEP)

.PHONY: all clean prog test run-test gen-test-coverage
