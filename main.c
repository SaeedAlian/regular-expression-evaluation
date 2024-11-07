#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stack {
  int top;
  int max;
  char *data;
} stack;

typedef struct nfa_transition {
  char symbol;
  struct nfa_state *next;
} nfa_transition;

typedef struct nfa_state {
  int id;
  int transitions_index;
  int transitions_len;
  struct nfa_transition *transitions;
} nfa_state;

stack *new_stack(int max) {
  struct stack *s = (stack *)malloc(sizeof(stack));
  s->data = (char *)malloc(sizeof(char) * max);
  s->top = -1;
  s->max = max;

  if (s->data == NULL)
    return NULL;

  return s;
}

void free_stack(stack *s) {
  free(s->data);
  free(s);
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

  case '.':
    return 2;

  case '*':
    return 3;

  default:
    return 0;
  }
}

char *standardize_regex(const char *regex, int len, int *new_len) {
  char *standard = (char *)malloc(sizeof(char) * (len * 2) + 1);

  int i = 0;
  int j = 0;

  while (i < len) {
    char curr = regex[i];
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

    if (curr == '*' || curr == ')') {
      if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z') ||
          (next >= '0' && next <= '9')) {
        standard[j++] = '.';
      }
    }

    i++;
  }

  standard[j++] = regex[len];
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
    } else if (c == '*' || c == '|' || c == '.') {
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
  free_stack(op);
  return postfix;
}

nfa_transition new_nfa_transition(char symbol, nfa_state *next) {
  nfa_transition t;
  t.symbol = symbol;
  t.next = next;

  return t;
}

nfa_state *new_nfa_state(int id) {
  nfa_state *state = (nfa_state *)malloc(sizeof(nfa_state));
  state->id = id;
  state->transitions_index = 0;
  state->transitions_len = 5;
  state->transitions =
      (nfa_transition *)malloc(sizeof(nfa_transition) * state->transitions_len);

  if (state->transitions == NULL)
    return NULL;

  return state;
}

int realloc_nfa_state_transitions(nfa_state *s) {
  s->transitions_len *= 2;
  s->transitions = (nfa_transition *)realloc(
      s->transitions, sizeof(nfa_transition) * s->transitions_len);

  if (s->transitions == NULL)
    return -1;

  return 0;
}

int add_nfa_transition(nfa_state *from, char symbol, nfa_state *to) {
  nfa_transition t = new_nfa_transition(symbol, to);

  if (from->transitions_index >= from->transitions_len - 1) {
    if (realloc_nfa_state_transitions(from) == -1) {
      return -1;
    }
  }

  from->transitions[from->transitions_index++] = t;
  return 0;
}

int main() {
  char *p = regex_to_postfix("a(b|a)*cb", 9);

  printf("%s\n", p);
  free(p);
  return 0;
}
