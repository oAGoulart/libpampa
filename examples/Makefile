CC=gcc
OS=UNIX
BIN=example

loader: *.c
	$(CC) -o $(BIN) *.c -D$(OS) -Wall -g

memcheck:
	valgrind --leak-check=yes ./$(BIN) -s
