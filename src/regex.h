#ifndef REGEX_H_
#define REGEX_H_

#include "nfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stack {
  int top;
  int max;
  char *data;
} stack;

void free_stack(stack *s);

stack *new_stack(int max);
int stack_is_full(stack *s);
int stack_is_empty(stack *s);
char stack_top(stack *s);
int stack_push(stack *s, char state);
int stack_pop(stack *s, char *state);

int operator_precedence(char op);
char *regex_to_postfix(const char *regex, int len);

char *standardize_regex(const char *regex, int len, int *new_len);

int evaluate_string(const char *str, const char *regex, int show_log);

#endif
