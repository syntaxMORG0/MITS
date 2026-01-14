CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = mits mits-interp
COMPILER_SRC = compiler.c
INTERPRETER_SRC = interpreter.c

all: mits mits-interp

mits: $(COMPILER_SRC)
	$(CC) $(CFLAGS) -o mits $(COMPILER_SRC)

mits-interp: $(INTERPRETER_SRC)
	$(CC) $(CFLAGS) -o mits-interp $(INTERPRETER_SRC)

clean:
	rm -f $(TARGETS) *.o

run: mits-interp
	./mits-interp main.s main.rom

.PHONY: all clean run
