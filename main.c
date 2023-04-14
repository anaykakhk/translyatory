#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// E ::= T E'
// E' ::= '|' T E' | e
// T ::= D T'
// T' ::= '&' D T' | e
// D::= H D'
// D'::= '^' H D' | e
// H::= ~F | F
// F ::= (E) | i

#define I 261
#define OR 262
#define AND 263
#define NOT 264
#define XOR 265
#define LPAREN 266
#define RPAREN 267

FILE *current_stream;
int token_value;

int scan() {
  int c;

  while ((c = fgetc(current_stream)) != EOF) {
    if (isspace(c)) {
      continue;
    }

    if (c == '|') {
      return OR;
    } else if (c == '&') {
      return AND;
    } else if (c == '^') {
      return XOR;
    } else if (c == '~') {
      return NOT;
    } else if (c == '(') {
      return LPAREN;
    } else if (c == ')') {
      return RPAREN;
    } else if (isdigit(c)) {
      token_value = c - '0';
      // int flag = 0;
      while (isdigit((c = fgetc(current_stream)))) {
        // flag = 1;
        token_value = token_value * 10 + c - '0';
      }
      // if (flag == 1)
      ungetc(c, current_stream);
      return I;
    } else {
      fprintf(stderr, "Неизвестный символ: %c\n", c);
      exit(1);
    }
  }

  return EOF;
}

//

int lookahead;

void match(int expected) {
  if (lookahead == expected) {
    lookahead = scan();
    printf("Лексема: %d\n", lookahead);
  } else {
    fprintf(stderr,
            "Ошибка синтаксического анализа: ожидался символ %d, получен %d\n",
            expected, lookahead);
    exit(1);
  }
}

int F();
int H();
int D();
int D_(int);
int T();
int T_(int);
int E();
int E_(int);

int F() {
  // F ::= (E) | i
  int result;

  if (lookahead == LPAREN) {
    match(LPAREN);
    result = E();
    match(RPAREN);
  } else if (lookahead == I) {
    result = token_value;
    match(I);
  } else {
    fprintf(stderr, "Syntax error in F()\n");
    printf("%d\n", lookahead);
    exit(1);
  }

  return result;
}

int H() {
  // H::= ~F | F
  int result;

  if (lookahead == NOT) {
    match(NOT);
    int tmp = F();
    printf("~ %d  = %d\n", tmp, ~tmp);
    result = ~tmp;
  } else {
    result = F();
  }

  return result;
}

int D() {
  // D::= H D'
  int left = H();
  return D_(left);
}

int D_(int left) {
  // D'::= '^' H D' | e
  int right;

  if (lookahead == XOR) {
    match(XOR);
    right = H();
    printf("%d ^ %d = %d\n", left, right, left ^ right);
    left ^= right;
    return D_(left);
  }

  return left;
}

int T() {
  // T ::= D T'
  int left = D();
  return T_(left);
}

int T_(int left) {
  // T' ::= '&' D T' | e
  int right;

  if (lookahead == AND) {
    match(AND);
    right = D();
    printf("%d & %d = %d\n", left, right, left & right);
    left &= right;
    return T_(left);
  }

  return left;
}

int E() {
  // E ::= T E'
  int left = T();
  return E_(left);
}

int E_(int left) {
  // E' ::= '|' T E' | e
  int right;

  if (lookahead == OR) {
    match(OR);
    right = T();
    printf("%d | %d = %d\n", left, right, left | right);
    left |= right;
    return E_(left);
  }

  return left;
}

void write_string_to_stdin(const char *str) {
  current_stream = fmemopen((void *)str, strlen(str), "r+");
}

int main() {

  // char *input = "(((1|2)&3)^4)"; // 7
  char *input = "~(1|2)&(3|4)"; // 4
  write_string_to_stdin(input);

  lookahead = scan();
  printf("lookahead:%d\n", lookahead);
  int result = E();
  printf("\n\nResult:%d\n\n", result);

  if (!(lookahead == EOF))
    exit(-100);

  printf("\n");

  return 0;
}
