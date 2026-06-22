CC = gcc

CFLAGS = -g

lexer: lexer.c
	    $(CC) $(CFLAGS) lexer.c  -lm
