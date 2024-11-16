#include "regex.h"

int evaluate_string(const char *str, const char *regex) {
  if (str == NULL) {
    printf("The provided string is empty!");
  }

  if (regex == NULL) {
    printf("The provided regex is empty!");
  }

  if (SHOW_LOG)
    printf("Evaluating '%s' with regular expression '%s'\n", str, regex);

  int standard_regex_len;
  int regex_len = strlen(regex);
  int str_len = strlen(str);

  char *standard = standardize_regex(regex, regex_len, &standard_regex_len);

  if (standard == NULL) {
    printf("There was an issue in the standardization process...");
  }

  if (SHOW_LOG)
    printf("Standardized regular expression: %s\n", standard);

  char *postfix = regex_to_postfix(standard, standard_regex_len);

  if (postfix == NULL) {
    printf("There was an issue in postfix creation process...");
  }

  int postfix_len = strlen(postfix);

  if (SHOW_LOG)
    printf("Postfix of regular expression: %s\n\n", postfix);

  nfa *n = new_nfa_from_regex(postfix, postfix_len);

  if (n == NULL) {
    printf("There was an issue in nfa creation process...");
  }

  if (SHOW_LOG) {
    print_nfa(n);
    printf("\n");
  }

  int evaluated = evaluate_string_in_nfa(n, str, str_len);

  printf("String '%s' is ", str);

  if (evaluated == 1) {
    printf("accepted");
  } else {
    printf("not accepted");
  }

  printf(" with the given regular expression\n");

  free_nfa(n);
  free(postfix);
  free(standard);
  return 0;
}

char *standardize_regex(const char *regex, int len, int *new_len) {
  char *standard = (char *)malloc(sizeof(char) * (len * 2) + 1);

  int i = 0;
  int j = 0;

  while (i < len) {
    char curr = regex[i];

    if (i == len - 1) {
      standard[j++] = curr;
      i++;
      continue;
    }

    char next = regex[i + 1];

    standard[j++] = curr;

    if ((curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z') ||
        (curr >= '0' && curr <= '9')) {
      if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z') ||
          (next >= '0' && next <= '9')) {
        standard[j++] = '.';
      }

      if (next == '(') {
        standard[j++] = '.';
      }
    }

    if (curr == '*') {
      if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z') ||
          (next >= '0' && next <= '9') || next == '(') {
        standard[j++] = '.';
      }
    }

    if (curr == ')') {
      if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z') ||
          (next >= '0' && next <= '9')) {
        standard[j++] = '.';
      }
    }

    i++;
  }

  standard[j] = '\0';
  (*new_len) = j;

  return standard;
}

char *regex_to_postfix(const char *regex, int len) {
  stack *op = new_stack(len);

  if (op == NULL) {
    return NULL;
  }

  char *postfix = (char *)malloc(sizeof(char) * (len + 1));

  if (postfix == NULL) {
    free_stack(op);
    return NULL;
  }

  int j = 0;
  for (int i = 0; i < len; i++) {
    char c = regex[i];

    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9')) {
      postfix[j++] = c;
    } else if (c == '(') {
      if (stack_push(op, c) == -1) {
        free_stack(op);
        free(postfix);
        return NULL;
      }
    } else if (c == ')') {
      while (!stack_is_empty(op) && stack_top(op) != '(') {
        char p;
        stack_pop(op, &p);
        postfix[j++] = p;
      }

      if (stack_top(op) == '(') {
        stack_pop(op, NULL);
      }
    } else if (c == '*' || c == '|' || c == '.') {
      char p;

      while (!stack_is_empty(op) &&
             (operator_precedence(c) <= operator_precedence(stack_top(op)))) {
        stack_pop(op, &p);
        postfix[j++] = p;
      }

      if (stack_push(op, c) == -1) {
        free_stack(op);
        free(postfix);
        return NULL;
      }
    } else {
      free_stack(op);
      free(postfix);
      return NULL;
    }
  }

  while (!stack_is_empty(op)) {
    char p;
    stack_pop(op, &p);
    postfix[j++] = p;
  }

  postfix[j] = '\0';
  free_stack(op);
  return postfix;
}

int operator_precedence(char op) {
  switch (op) {
  case '|':
    return 1;

  case '.':
    return 2;

  case '*':
    return 3;

  default:
    return 0;
  }
}

stack *new_stack(int max) {
  stack *s = (stack *)malloc(sizeof(stack));
  s->data = (char *)malloc(sizeof(char) * max);
  s->top = -1;
  s->max = max;

  if (s->data == NULL) {
    if (s != NULL)
      free(s);

    return NULL;
  }

  return s;
}

void free_stack(stack *s) {
  free(s->data);
  free(s);
}

int stack_is_full(stack *s) { return s->top == s->max - 1; }
int stack_is_empty(stack *s) { return s->top == -1; }

char stack_top(stack *s) { return s->data[s->top]; }

int stack_push(stack *s, char state) {
  int new_top = ++s->top;

  if (new_top > s->max - 1)
    return -1;

  s->data[new_top] = state;

  return 0;
}

int stack_pop(stack *s, char *state) {
  if (s->top < 0)
    return -1;

  if (state != NULL) {
    (*state) = s->data[s->top];
  }

  s->top--;
  return 0;
}
