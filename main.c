#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum tokens {
  CHAR = 1,
  OPEN_PARENTHESIS = 2,
  CLOSE_PARENTHESIS = 3,
  PLUS = 4,
  STAR = 5,
  SLASH = 6,
};

int *tokenizer(const char *re, int re_len) {
  int *tokens_list = (int *)malloc(sizeof(int) * re_len);

  for (int i = 0; i < re_len; i++) {
    char c = re[i];

    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
      tokens_list[i] = CHAR;
    } else if (c == '(') {
      tokens_list[i] = OPEN_PARENTHESIS;
    } else if (c == ')') {
      tokens_list[i] = CLOSE_PARENTHESIS;
    } else if (c == '+') {
      tokens_list[i] = PLUS;
    } else if (c == '*') {
      tokens_list[i] = STAR;
    } else if (c == '/') {
      tokens_list[i] = SLASH;
    } else {
      return NULL;
    }
  }

  if (tokens_list[0] != SLASH || tokens_list[re_len - 1] != SLASH)
    return NULL;

  return tokens_list;
}

int regex_match(const char *str, const char *re) {
  int re_len = strlen(re);
  int *tokens_list = tokenizer(re, re_len);

  for (int i = 0; i < re_len; i++) {
    printf("-- %d --", tokens_list[i]);
  }

  free(tokens_list);
  return 0;
}

int main() {
  int res = regex_match("abbacb", "/a(b+a)*cb/");
  printf("%d\n", res);
  return 0;
}
