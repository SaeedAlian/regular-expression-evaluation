#ifndef REGEX_H_
#define REGEX_H_

#define SHOW_LOG 1

#include "nfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stack {
  int top;
  int max;
  char *data;
} stack;

stack *new_stack(int max);
void free_stack(stack *s);
int stack_is_full(struct stack *s);
int stack_is_empty(struct stack *s);
char stack_top(struct stack *s);
int stack_push(struct stack *s, char state);
int stack_pop(struct stack *s, char *state);
int operator_precedence(char op);
char *standardize_regex(const char *regex, int len, int *new_len);
char *regex_to_postfix(const char *regex, int len);
int evaluate_string(const char *str, const char *regex);

#endif
