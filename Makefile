CFLAGS = -std=gnu99 -g -Wall -Wextra

.PHONY: all

main: bowl.o

main.c: bowl.h

test: bowl.o

test.o: bowl.h attotest.h

all: main

.PHONY: run-test
run-test: test
	@./test || exit 0

.PHONY: clean
clean:
	-rm test main *.o
