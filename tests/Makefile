CC = gcc

CFLAGS = -g -lm -gz=none

main: test_parser.c
		$(CC) $(CFLAGS)   ./test_parser.c  ../src/lexer.c ../src/parser.c ../src/variable_store.c ../src/utils/math_utils.c ../src/utils/string_view_utils.c ../src/utils/parser_utils.c ../src/utils/lexer_utils.c ../src/utils/hashmap_utils.c ../src/utils/ll_utils.c  -o tests.out
