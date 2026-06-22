#include <ctype.h>
#include <math.h>
#include <stdio.h>

// an arithmetic lexer

typedef enum {
  NUMBER,
  PLUS,
  MINUS,
  ASTERISK,
  SLASH,
  END,
  UNKNOWN,
} TokenKind;

typedef struct {
  double num;
  TokenKind kind;
} Token;

typedef struct {
  char *string;
  size_t cursor;
} Lexer;

Token tokenise(Lexer *lexer);

int main() {

  printf("hi\n");
  int test = 1;
  char testString[] = "123-2*4";

  Lexer lexer = {
      .string = testString,
      .cursor = 0,
  };

  tokenise(&lexer);

  return 0;
}

Token tokenise(Lexer *lexer) {
  Token token;

  switch (lexer->string[lexer->cursor]) {
  case '\n': {
    token.kind = END;
    break;
  }

  case '0' ... '9': {
    token.num = 0;
    token.kind = NUMBER;

    int i;
    for (i = lexer->cursor; isdigit(lexer->string[i]); i++) {
      token.num *= 10;
      token.num += (lexer->string[i] - '0');
    }

    lexer->cursor = i;
    break;
  }

  case '+': {
    token.kind = PLUS;
    lexer->cursor++;
    break;
  }

  case '-': {
    token.kind = MINUS;
    lexer->cursor++;

    break;
  }

  case '*': {
    token.kind = SLASH;
    lexer->cursor++;
    break;
  }

  case '/': {
    token.kind = SLASH;
    lexer->cursor++;
    break;
  }
  }

  return token;
}
