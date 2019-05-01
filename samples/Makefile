CC=gcc
OS=UNIX
BIN=sample

loader: *.c
	$(CC) -o $(BIN) *.c -D$(OS) -Wall -g

memcheck:
	valgrind --leak-check=yes ./$(BIN) -s
