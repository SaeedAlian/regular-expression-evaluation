#ifndef NFA_H_
#define NFA_H_

#include <stdio.h>
#include <stdlib.h>

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

typedef struct nfa_stack {
  int top;
  int max;
  nfa *data;
} nfa_stack;

typedef struct nfa_state_stack {
  int top;
  int max;
  nfa_state **data;
} nfa_state_stack;

nfa_stack *new_nfa_stack(int max);
void free_nfa_stack(nfa_stack *s);
int nfa_stack_is_full(nfa_stack *s);
int nfa_stack_is_empty(nfa_stack *s);
nfa nfa_stack_top(nfa_stack *s);
int nfa_stack_push(nfa_stack *s, nfa state);
int nfa_stack_pop(nfa_stack *s, nfa *state);
nfa_state_stack *new_nfa_state_stack(int max);
void free_nfa_state_stack(nfa_state_stack *s);
int nfa_state_stack_is_full(nfa_state_stack *s);
int nfa_state_stack_is_empty(nfa_state_stack *s);
nfa_state *nfa_state_stack_top(nfa_state_stack *s);
int nfa_state_stack_push(nfa_state_stack *s, nfa_state *state);
int nfa_state_stack_pop(nfa_state_stack *s, nfa_state **state);
nfa_state *new_nfa_state(int id);
int add_nfa_transition(nfa_state *from, nfa_state *to, char symbol);
int add_epsilon_nfa_transition(nfa_state *from, nfa_state *to);
nfa *new_nfa_from_regex(const char *regex, int len);
void free_nfa_state(nfa_state *s, int *visited);
void free_nfa(nfa *n);
void print_nfa_state(nfa_state *state, int *visited);
void print_nfa(nfa *nfa);

#endif
