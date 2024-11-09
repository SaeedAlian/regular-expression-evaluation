#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nfa_state {
  int id;
  char symbol;
  struct nfa_state *next;
  struct nfa_state *epsilon;
} nfa_state;

typedef struct nfa {
  int number_of_states;
  nfa_state *init;
  nfa_state *final;
} nfa;

typedef struct stack {
  int top;
  int max;
  char *data;
} stack;

typedef struct nfa_stack {
  int top;
  int max;
  nfa *data;
} nfa_stack;

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

nfa_stack *new_nfa_stack(int max) {
  struct nfa_stack *s = (nfa_stack *)malloc(sizeof(nfa_stack));
  s->data = (nfa *)malloc(sizeof(nfa) * max);
  s->top = -1;
  s->max = max;

  if (s->data == NULL)
    return NULL;

  return s;
}

void free_nfa_stack(nfa_stack *s) {
  free(s->data);
  free(s);
}

int stack_is_full(struct stack *s) { return s->top == s->max - 1; }
int stack_is_empty(struct stack *s) { return s->top == -1; }

int nfa_stack_is_full(struct nfa_stack *s) { return s->top == s->max - 1; }
int nfa_stack_is_empty(struct nfa_stack *s) { return s->top == -1; }

char stack_top(struct stack *s) { return s->data[s->top]; }
nfa nfa_stack_top(struct nfa_stack *s) { return s->data[s->top]; }

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

int nfa_stack_push(struct nfa_stack *s, nfa state) {
  int new_top = ++s->top;

  if (new_top > s->max - 1)
    return -1;

  s->data[new_top] = state;

  return 0;
}

int nfa_stack_pop(struct nfa_stack *s, nfa *state) {
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

nfa_state *new_nfa_state(int id) {
  nfa_state *state = (nfa_state *)malloc(sizeof(nfa_state));
  state->id = id;
  state->symbol = '\0';
  state->next = NULL;
  state->epsilon = NULL;

  return state;
}

int add_nfa_transition(nfa_state *from, nfa_state *to, char symbol) {
  if (from->next != NULL)
    return -1;

  from->next = to;
  from->symbol = symbol;
  return 0;
}

int add_epsilon_nfa_transition(nfa_state *from, nfa_state *to) {
  if (from->epsilon != NULL) {
    return add_nfa_transition(from, to, '\0');
  }

  from->epsilon = to;
  return 0;
}

nfa *new_nfa_from_regex(const char *regex, int len) {
  int count = 0;
  int transition_err = 0;
  nfa_stack *s = new_nfa_stack(len);

  // if regex is empty, then we have a epsilon regex, which is just
  // a epsilon transition from init state to final state.
  if (len == 0) {
    nfa n;
    nfa_state *init = new_nfa_state(count++);
    nfa_state *final = new_nfa_state(count++);

    transition_err = add_epsilon_nfa_transition(init, final);
    if (transition_err == -1)
      return NULL;

    n.init = init;
    n.final = final;
    n.number_of_states = 2;

    nfa_stack_push(s, n);
  }

  int i = 0;
  while (i < len) {
    char c = regex[i];
    i++;

    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9')) {
      nfa n;
      nfa_state *init = new_nfa_state(count++);
      nfa_state *final = new_nfa_state(count++);

      transition_err = add_nfa_transition(init, final, c);

      if (transition_err == -1)
        return NULL;

      n.init = init;
      n.final = final;
      n.number_of_states = 2;

      nfa_stack_push(s, n);
    } else if (c == '*') {
      nfa last;

      if (nfa_stack_is_empty(s)) {
        return NULL;
      }

      nfa_stack_pop(s, &last);

      nfa n;
      nfa_state *new_state = new_nfa_state(count++);

      transition_err = add_epsilon_nfa_transition(new_state, last.init);
      transition_err = add_epsilon_nfa_transition(new_state, last.final);
      transition_err = add_epsilon_nfa_transition(last.final, new_state);

      if (transition_err == -1)
        return NULL;

      n.init = new_state;
      n.final = last.final;
      n.number_of_states = last.number_of_states + 1;
      nfa_stack_push(s, n);
    } else if (c == '.') {
      nfa l1;
      nfa l2;

      if (nfa_stack_is_empty(s)) {
        return NULL;
      }

      nfa_stack_pop(s, &l1);

      if (nfa_stack_is_empty(s)) {
        return NULL;
      }

      nfa_stack_pop(s, &l2);

      nfa n;
      transition_err = add_epsilon_nfa_transition(l2.final, l1.init);

      if (transition_err == -1)
        return NULL;

      n.init = l2.init;
      n.final = l1.final;
      n.number_of_states = l1.number_of_states + l2.number_of_states;
      nfa_stack_push(s, n);
    } else if (c == '|') {
      nfa l1;
      nfa l2;

      if (nfa_stack_is_empty(s)) {
        return NULL;
      }

      nfa_stack_pop(s, &l1);

      if (nfa_stack_is_empty(s)) {
        return NULL;
      }

      nfa_stack_pop(s, &l2);

      nfa n;
      nfa_state *init = new_nfa_state(count++);
      nfa_state *final = new_nfa_state(count++);

      transition_err = add_epsilon_nfa_transition(init, l1.init);
      transition_err = add_epsilon_nfa_transition(init, l2.init);
      transition_err = add_epsilon_nfa_transition(l2.final, final);
      transition_err = add_epsilon_nfa_transition(l1.final, final);

      if (transition_err == -1)
        return NULL;

      n.init = init;
      n.final = final;
      n.number_of_states = l1.number_of_states + l2.number_of_states + 2;
      nfa_stack_push(s, n);
    } else {
      return NULL;
    }
  }

  if (nfa_stack_is_empty(s)) {
    return NULL;
  }

  nfa *n = (nfa *)malloc(sizeof(nfa));
  nfa_stack_pop(s, n);

  if (!nfa_stack_is_empty(s)) {
    return NULL;
  }

  free_nfa_stack(s);
  return n;
}

void print_nfa_state(nfa_state *state, int *visited) {
  if (visited[state->id] != 0) {
    return;
  }

  printf("q%i     ", state->id);

  if (state->symbol != '\0' && state->next != NULL) {
    printf(" | %c -> q%i | ", state->symbol, state->next->id);
  }

  if (state->symbol == '\0' && state->next != NULL) {
    printf(" | eps -> q%i | ", state->next->id);
  }

  if (state->epsilon != NULL) {
    printf(" | eps -> q%i | ", state->epsilon->id);
  }

  visited[state->id] = 1;

  if (state->next != NULL && state->epsilon != NULL &&
      state->epsilon->id != state->next->id) {
    printf("\n");
    print_nfa_state(state->next, visited);
    print_nfa_state(state->epsilon, visited);
  } else if (state->next != NULL) {
    printf("\n");
    print_nfa_state(state->next, visited);
  } else if (state->epsilon != NULL) {
    printf("\n");
    print_nfa_state(state->epsilon, visited);
  } else {
    printf(" | NULL | ");
    printf("\n");
    return;
  }
}

void print_nfa(nfa *nfa) {
  printf("NFA:\n");
  printf("Initial State -> %i\n", nfa->init->id);
  printf("Final State -> %i\n", nfa->final->id);
  printf("-----------------------------------------\n");
  int *visited = (int *)malloc(sizeof(int) * nfa->number_of_states);

  for (int i = 0; i < nfa->number_of_states; i++)
    visited[i] = 0;

  print_nfa_state(nfa->init, visited);
  free(visited);
}

int main() {
  const char *d = "a(b|a)*cb*";
  int i;
  int l = strlen(d);
  char *s = standardize_regex(d, l, &i);
  char *p = regex_to_postfix(s, i);

  nfa *n = new_nfa_from_regex(p, strlen(p));
  print_nfa(n);

  free(n);
  free(p);
  free(s);
  return 0;
}
