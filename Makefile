CC = gcc

CFLAGS = -g -lm

main: main.c
		$(CC) $(CFLAGS)  -o main.out -g main.c -g lexer.c -g parser.c

# lexer: lexer.c
# 	    $(CC) $(CFLAGS) lexer.c  -lm
#
# parser: parser.c
#  	    $(CC) $(CFLAGS) parser.c  -lm
