CC = gcc

CFLAGS = -g -lm -gz=none

main: main.c
		$(CC) $(CFLAGS)   main.c lexer.c parser.c -o main.out
