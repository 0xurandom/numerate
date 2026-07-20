CC = gcc

CFLAGS = -g -lm -lmpfr -lgmp -gz=none -Wall -Wextra

main: main.c
		$(CC) $(CFLAGS)   main.c lexer.c parser.c variable_store.c utils/math_utils.c utils/string_view_utils.c utils/parser_utils.c utils/lexer_utils.c utils/hashmap_utils.c utils/ll_utils.c  -o main.out
