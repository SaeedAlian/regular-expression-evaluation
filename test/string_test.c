#include "../src/regex.h"
#include <sys/time.h>

int test_strings(const char *str, const char *regex, int expected_val);

void test();

int main() {
  test();
  return 0;
}

void test() {
  printf("Testing strings...\n");

  int tests[40] = {-1};
  int total = 40;
  int success = 0;

  double total_time = 0;

  for (int i = 0; i < 40; i++)
    tests[i] = -1;

  const char *tests_string_inputs[40];
  const char *tests_regex_inputs[40];
  int tests_expected_returns[40];

  for (int i = 0; i < 40; i++) {
    tests_string_inputs[i] = "";
    tests_regex_inputs[i] = "";
    tests_expected_returns[i] = 1;
  }

  tests_string_inputs[0] = "abababab";
  tests_regex_inputs[0] = "(ab)*";
  tests_expected_returns[0] = 1;

  tests_string_inputs[1] = "abababab";
  tests_regex_inputs[1] = "ab(ab)*";
  tests_expected_returns[1] = 1;

  tests_string_inputs[2] = "bababab";
  tests_regex_inputs[2] = "ab(ab)*";
  tests_expected_returns[2] = 0;

  tests_string_inputs[3] = "ab";
  tests_regex_inputs[3] = "ab(ab)*";
  tests_expected_returns[3] = 1;

  tests_string_inputs[4] = "ab";
  tests_regex_inputs[4] = "(ab)*|(df)*(ab)*";
  tests_expected_returns[4] = 1;

  tests_string_inputs[5] = "abdf";
  tests_regex_inputs[5] = "(ab)*|(df)*(ab)*";
  tests_expected_returns[5] = 0;

  tests_string_inputs[5] = "dfdfdfabab";
  tests_regex_inputs[5] = "(ab)*|(df)*(ab)*";
  tests_expected_returns[5] = 1;

  tests_string_inputs[6] = "dfdf";
  tests_regex_inputs[6] = "(ab)*|(df)*(ab)*";
  tests_expected_returns[6] = 1;

  tests_string_inputs[7] = "ghghaaabbababhghgf";
  tests_regex_inputs[7] = "ghgh(g|h|f|a|b)*";
  tests_expected_returns[7] = 1;

  tests_string_inputs[8] = "hghaaabbababhghgf";
  tests_regex_inputs[8] = "ghgh(g|h|f|a|b)*";
  tests_expected_returns[8] = 0;

  tests_string_inputs[9] = "a";
  tests_regex_inputs[9] = "a|(bc|df)*mdm*(nbn)*|d*wd*";
  tests_expected_returns[9] = 1;

  tests_string_inputs[10] = "bcbcbcmdnbnnbn";
  tests_regex_inputs[10] = "a|(bc|df)*mdm*(nbn)*|d*wd*";
  tests_expected_returns[10] = 1;

  tests_string_inputs[11] = "bcbcdmdnbn";
  tests_regex_inputs[11] = "a|(bc|df)*mdm*(nbn)*|d*wd*";
  tests_expected_returns[11] = 0;

  for (int i = 0; i < 40; i++) {
    if (strlen(tests_regex_inputs[i]) == 0 ||
        strlen(tests_string_inputs[i]) == 0) {
      total--;
      continue;
    }

    printf("T%i Testing...\n", i + 1);

    struct timeval s, e;
    gettimeofday(&s, NULL);

    int t = test_strings(tests_string_inputs[i], tests_regex_inputs[i],
                         tests_expected_returns[i]);
    gettimeofday(&e, NULL);

    if (t == tests_expected_returns[i]) {
      printf("T%i is successful", i + 1);
      success++;
      tests[i] = 1;
    } else {
      printf("T%i has failed", i + 1);
      tests[i] = 0;
    }

    double diff = (e.tv_sec - s.tv_sec) * 1000.0;
    diff += (e.tv_usec - s.tv_usec);
    total_time += diff;
    printf(" --- duration: %.3lf ms\n", diff);
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

  printf("Finish testing strings\n");
  printf("Total time: %.3lf ms\n", total_time);
  printf("Avg time: %.3lf ms\n", total_time / total);
  printf("\n");
}

int test_strings(const char *str, const char *regex, int expected_val) {
  printf("Testing string '%s' with regex '%s'...\n", str, regex);
  int val = evaluate_string(str, regex, 0);
  return val;
}
