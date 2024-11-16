#include "../src/regex.h"

int test_postfix(const char *regex, const char *expected_val);
int test_standardize(const char *regex, const char *expected_val,
                     int expected_len);

void test();

int main() {
  test();
  return 0;
}

void test() {
  printf("Testing regex...\n");

  int tests[40];
  int total = 40;
  int success = 0;

  for (int i = 0; i < 40; i++)
    tests[i] = -1;

  const char *postfix_tests_inputs[20];
  const char *postfix_tests_expected_values[20];
  int postfix_tests_expected_returns[20];

  for (int i = 0; i < 20; i++) {
    postfix_tests_inputs[i] = "";
    postfix_tests_expected_values[i] = "";
    postfix_tests_expected_returns[i] = 1;
  }

  postfix_tests_inputs[0] = "a.b";
  postfix_tests_expected_values[0] = "ab.";
  postfix_tests_expected_returns[0] = 1;

  postfix_tests_inputs[1] = "a.b.c|d|d";
  postfix_tests_expected_values[1] = "ab.c.d|d|";
  postfix_tests_expected_returns[1] = 1;

  postfix_tests_inputs[2] = "a.(a|b)*.c|d|(f|d)*";
  postfix_tests_expected_values[2] = "aab|*.c.d|fd|*|";
  postfix_tests_expected_returns[2] = 1;

  postfix_tests_inputs[3] = "a.b.c.(b)*";
  postfix_tests_expected_values[3] = "ab.c.b*.";
  postfix_tests_expected_returns[3] = 1;

  for (int i = 0; i < 20; i++) {
    if (strlen(postfix_tests_inputs[i]) == 0 ||
        strlen(postfix_tests_expected_values[i]) == 0) {
      total--;
      continue;
    }

    printf("T%i Testing...\n", i + 1);
    int t =
        test_postfix(postfix_tests_inputs[i], postfix_tests_expected_values[i]);

    if (t == postfix_tests_expected_returns[i]) {
      printf("T%i is successful\n", i + 1);
      success++;
      tests[i] = 1;
    } else {
      printf("T%i has failed\n", i + 1);
      tests[i] = 0;
    }
  }

  const char *standardize_tests_inputs[20];
  const char *standardize_tests_expected_values[20];
  int standardize_tests_expected_value_lens[20];
  int standardize_tests_expected_returns[20];

  for (int i = 0; i < 20; i++) {
    standardize_tests_inputs[i] = "";
    standardize_tests_expected_values[i] = "";
    standardize_tests_expected_value_lens[i] = 0;
    standardize_tests_expected_returns[i] = 1;
  }

  standardize_tests_inputs[0] = "a.bcc|(dd)*";
  standardize_tests_expected_values[0] = "a.b.c.c|(d.d)*";
  standardize_tests_expected_value_lens[0] = 14;
  standardize_tests_expected_returns[0] = 1;

  standardize_tests_inputs[1] = "abddbdbd(d|d|dddl)*dk*(d)*";
  standardize_tests_expected_values[1] =
      "a.b.d.d.b.d.b.d.(d|d|d.d.d.l)*.d.k*.(d)*";
  standardize_tests_expected_value_lens[1] = 40;
  standardize_tests_expected_returns[1] = 1;

  for (int i = 0; i < 20; i++) {
    if (strlen(standardize_tests_inputs[i]) == 0 ||
        strlen(standardize_tests_expected_values[i]) == 0) {
      total--;
      continue;
    }

    printf("T%i Testing...\n", i + 20 + 1);
    int t = test_standardize(standardize_tests_inputs[i],
                             standardize_tests_expected_values[i],
                             standardize_tests_expected_value_lens[i]);
    if (t == standardize_tests_expected_returns[i]) {
      printf("T%i is successful\n", i + 20 + 1);
      success++;
      tests[i + 20] = 1;
    } else {
      printf("T%i has failed\n", i + 20 + 1);
      tests[i + 20] = 0;
    }
  }

  if (success == total) {
    printf("All tests were successful\n");
  } else {
    printf("Some tests are failed:\n");
    for (int i = 0; i < 40; i++) {
      if (tests[i] == 0)
        printf("  T%i  ", i + 1);
    }

    printf("\n");
  }

  printf("Finish testing regex\n\n");
}

int test_postfix(const char *regex, const char *expected_val) {
  printf("Testing regex '%s' for postfix...\n", regex);
  int len = strlen(regex);
  char *postfix = regex_to_postfix(regex, len);

  if (strcmp(postfix, expected_val) == 0) {
    return 1;
  }

  free(postfix);

  return 0;
}

int test_standardize(const char *regex, const char *expected_val,
                     int expected_len) {
  printf("Testing regex '%s' for standardization...\n", regex);
  int len = strlen(regex);
  int slen;
  char *standardized = standardize_regex(regex, len, &slen);

  if (strcmp(standardized, expected_val) == 0 && slen == expected_len) {
    return 1;
  }

  free(standardized);

  return 0;
}
