#include "regex.h"

int main() {
  const char *r = "a(a|b)*(bcb)*|b*";
  const char *s = "abbbbcb";

  evaluate_string(s, r);
  return 0;
}
