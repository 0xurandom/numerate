CC = gcc

CFLAGS = -g -lm -gz=none

main: test_parser.c
		$(CC) $(CFLAGS)   ./test_parser.c  ../src/lexer.c ../src/parser.c ../src/math_utils.c -o tests.out
