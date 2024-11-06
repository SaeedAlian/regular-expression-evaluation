#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stack {
  int top;
  int max;
  char *data;
} stack;

stack *new_stack(int max) {
  struct stack *s = (stack *)malloc(sizeof(stack));
  s->data = (char *)malloc(sizeof(char) * max);
  s->top = -1;
  s->max = max;

  if (s->data == NULL)
    return NULL;

  return s;
}

int stack_is_full(struct stack *s) { return s->top == s->max - 1; }
int stack_is_empty(struct stack *s) { return s->top == -1; }

char stack_top(struct stack *s) { return s->data[s->top]; }

int stack_push(struct stack *s, char state) {
  int new_top = ++s->top;

  if (new_top > s->max - 1)
    return -1;

  s->data[new_top] = state;

  return 0;
}

int stack_pop(struct stack *s, char *state) {
  if (s->top < 0)
    return -1;

  if (state != NULL) {
    (*state) = s->data[s->top];
  }

  s->top--;
  return 0;
}

int operator_precedence(char op) {
  switch (op) {
  case '|':
    return 1;

  case '*':
    return 2;

  default:
    return 0;
  }
}

char *regex_to_postfix(const char *regex, int len) {
  stack *op = new_stack(len);

  if (op == NULL) {
    return NULL;
  }

  char *postfix = (char *)malloc(sizeof(char) * (len + 1));

  if (postfix == NULL) {
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
    } else if (c == '*' || c == '|') {
      char p = stack_top(op);
      while (!stack_is_empty(op) &&
             (operator_precedence(c) <= operator_precedence(p))) {
        stack_pop(op, &p);
        postfix[j++] = p;
      }

      if (stack_push(op, c) == -1) {
        return NULL;
      }
    } else {
      continue;
    }
  }

  while (!stack_is_empty(op)) {
    char p;
    stack_pop(op, &p);
    postfix[j++] = p;
  }

  postfix[j] = '\0';
  free(op);
  return postfix;
}

int main() {
  char *p = regex_to_postfix("a(b|a)*cb", 9);

  printf("%s\n", p);
  free(p);
  return 0;
}
