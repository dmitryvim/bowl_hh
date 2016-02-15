CFLAGS = -std=gnu99 -g -Wall -Wextra

.PHONY: all clean

main: bowl.o

all: main

clean:
	-rm main *.o
