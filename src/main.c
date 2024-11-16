#include "regex.h"
#include "util.h"

int main() {
  char regex[1024];
  char str[1024];

  int c;
  c = get_input("Enter Regular Expression: ", regex, sizeof(regex));

  if (c == EMPTY_INPUT || c == NO_INPUT) {
    printf("No input provided for regular expression\n");
    return -1;
  }

  if (c == TOO_LONG) {
    printf("Regular expression is too long\n");
    return -1;
  }

  c = get_input("Enter string: ", str, sizeof(str));

  if (c == NO_INPUT) {
    printf("No input provided for string\n");
    return -1;
  }

  if (c == TOO_LONG) {
    printf("String is too long\n");
    return -1;
  }

  int e = evaluate_string(str, regex, 0);

  printf("String '%s' is ", str);

  if (e == 1) {
    printf("accepted");
  } else if (e == 0) {
    printf("not accepted");
  } else {
    printf("There was an issue in evaluation process...");
    return -1;
  }

  printf(" with the given regular expression\n");

  return 0;
}
