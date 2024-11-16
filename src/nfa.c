#include "nfa.h"

nfa_stack *new_nfa_stack(int max) {
  nfa_stack *s = (nfa_stack *)malloc(sizeof(nfa_stack));

  if (s == NULL)
    return NULL;

  s->data = (nfa *)malloc(sizeof(nfa) * max);
  s->top = -1;
  s->max = max;

  if (s->data == NULL) {
    if (s != NULL)
      free(s);

    return NULL;
  }

  return s;
}

void free_nfa_stack(nfa_stack *s) {
  free(s->data);
  free(s);
}

int nfa_stack_is_full(nfa_stack *s) { return s->top == s->max - 1; }
int nfa_stack_is_empty(nfa_stack *s) { return s->top == -1; }

nfa nfa_stack_top(nfa_stack *s) { return s->data[s->top]; }

int nfa_stack_push(nfa_stack *s, nfa state) {
  int new_top = ++s->top;

  if (new_top > s->max - 1)
    return -1;

  s->data[new_top] = state;

  return 0;
}

int nfa_stack_pop(nfa_stack *s, nfa *state) {
  if (s->top < 0)
    return -1;

  if (state != NULL) {
    (*state) = s->data[s->top];
  }

  s->top--;
  return 0;
}

nfa_state_stack *new_nfa_state_stack(int max) {
  nfa_state_stack *s = (nfa_state_stack *)malloc(sizeof(nfa_state_stack));

  if (s == NULL)
    return NULL;

  s->data = (nfa_state **)malloc(sizeof(nfa_state *) * max);
  s->top = -1;
  s->max = max;

  if (s->data == NULL) {
    if (s != NULL)
      free(s);

    return NULL;
  }

  return s;
}

void free_nfa_state_stack(nfa_state_stack *s) {
  free(s->data);
  free(s);
}

int nfa_state_stack_is_full(nfa_state_stack *s) { return s->top == s->max - 1; }
int nfa_state_stack_is_empty(nfa_state_stack *s) { return s->top == -1; }

nfa_state *nfa_state_stack_top(nfa_state_stack *s) { return s->data[s->top]; }

int nfa_state_stack_push(nfa_state_stack *s, nfa_state *state) {
  int new_top = ++s->top;

  if (new_top > s->max - 1)
    return -1;

  s->data[new_top] = state;

  return 0;
}

int nfa_state_stack_pop(nfa_state_stack *s, nfa_state **state) {
  if (s->top < 0)
    return -1;

  if (state != NULL) {
    (*state) = s->data[s->top];
  }

  s->top--;
  return 0;
}

nfa_state_queue *new_nfa_state_queue(int max) {
  nfa_state_queue *q = (nfa_state_queue *)malloc(sizeof(nfa_state_queue));

  if (q == NULL)
    return NULL;

  q->data = (nfa_state **)malloc(sizeof(nfa_state *) * max);
  q->front = -1;
  q->rear = -1;
  q->max = max;

  if (q->data == NULL) {
    if (q != NULL)
      free(q);

    return NULL;
  }

  return q;
}

void free_nfa_state_queue(nfa_state_queue *q) {
  free(q->data);
  free(q);
}

int nfa_state_queue_is_full(nfa_state_queue *q) {
  return (q->rear + 1) % q->max == q->front;
}

int nfa_state_queue_is_empty(nfa_state_queue *q) { return q->front == -1; }

nfa_state *nfa_state_queue_front(nfa_state_queue *q) {
  return q->data[q->front];
}

int nfa_state_queue_enqueue(nfa_state_queue *q, nfa_state *state) {
  if ((q->rear + 1) % q->max == q->front)
    return -1;

  int new_rear = (q->rear + 1) % q->max;

  if (q->front == -1)
    q->front = (q->front + 1) % q->max;

  q->data[new_rear] = state;
  q->rear = new_rear;

  return 0;
}

int nfa_state_queue_dequeue(nfa_state_queue *q, nfa_state **state) {
  if (q->front == -1)
    return -1;

  if (state != NULL)
    (*state) = q->data[q->front];

  if (q->front == q->rear) {
    q->front = q->rear = -1;
  } else {
    q->front = (q->front + 1) % q->max;
  }

  return 0;
}

int nfa_state_queue_length(nfa_state_queue *q) {
  if (q->front == -1 && q->rear == -1)
    return 0;

  int len = q->rear - q->front + 1;

  if (len < 0)
    return -len;
  return len;
}

nfa_state *new_nfa_state(int id) {
  nfa_state *state = (nfa_state *)malloc(sizeof(nfa_state));

  if (state == NULL)
    return NULL;

  state->id = id;
  state->symbol = '\0';
  state->next = NULL;
  state->epsilon = NULL;

  return state;
}

int add_nfa_transition(nfa_state *from, nfa_state *to, char symbol) {
  if (from == NULL || to == NULL)
    return -1;
  if (from->next != NULL)
    return -1;

  from->next = to;
  from->symbol = symbol;
  return 0;
}

int add_epsilon_nfa_transition(nfa_state *from, nfa_state *to) {
  if (from == NULL || to == NULL)
    return -1;
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

  if (s == NULL)
    return NULL;

  // if regex is empty, then we have a epsilon regex, which is just
  // a epsilon transition from init state to final state.
  if (len == 0) {
    nfa n;
    nfa_state *init = new_nfa_state(count++);
    nfa_state *final = new_nfa_state(count++);

    transition_err = add_epsilon_nfa_transition(init, final);
    if (transition_err == -1) {
      free_nfa_stack(s);
      return NULL;
    }

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

      if (transition_err == -1) {
        free_nfa_stack(s);
        return NULL;
      }

      n.init = init;
      n.final = final;
      n.number_of_states = 2;

      nfa_stack_push(s, n);
    } else if (c == '*') {
      nfa last;

      if (nfa_stack_is_empty(s)) {
        free_nfa_stack(s);
        return NULL;
      }

      nfa_stack_pop(s, &last);

      nfa n;

      if ((last.final->epsilon != NULL && last.final->next != NULL) ||
          (last.init->epsilon != NULL && last.init->next != NULL)) {
        nfa_state *new_state = new_nfa_state(count++);

        transition_err = add_epsilon_nfa_transition(new_state, last.init);
        transition_err = add_epsilon_nfa_transition(new_state, last.final);
        transition_err = add_epsilon_nfa_transition(last.final, new_state);

        n.init = new_state;
        n.final = last.final;
        n.number_of_states = last.number_of_states + 1;
      } else {
        transition_err = add_epsilon_nfa_transition(last.final, last.init);
        transition_err = add_epsilon_nfa_transition(last.init, last.final);

        n.init = last.init;
        n.final = last.final;
        n.number_of_states = last.number_of_states;
      }

      if (transition_err == -1) {
        free_nfa_stack(s);
        return NULL;
      }

      nfa_stack_push(s, n);
    } else if (c == '.') {
      nfa l1;
      nfa l2;

      if (nfa_stack_is_empty(s)) {
        free_nfa_stack(s);
        return NULL;
      }

      nfa_stack_pop(s, &l1);

      if (nfa_stack_is_empty(s)) {
        free_nfa_stack(s);
        return NULL;
      }

      nfa_stack_pop(s, &l2);

      nfa n;
      transition_err = add_epsilon_nfa_transition(l2.final, l1.init);

      if (transition_err == -1) {
        free_nfa_stack(s);
        return NULL;
      }

      n.init = l2.init;
      n.final = l1.final;
      n.number_of_states = l1.number_of_states + l2.number_of_states;
      nfa_stack_push(s, n);
    } else if (c == '|') {
      nfa l1;
      nfa l2;

      if (nfa_stack_is_empty(s)) {
        free_nfa_stack(s);
        return NULL;
      }

      nfa_stack_pop(s, &l1);

      if (nfa_stack_is_empty(s)) {
        free_nfa_stack(s);
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

      if (transition_err == -1) {
        free_nfa_stack(s);
        return NULL;
      }

      n.init = init;
      n.final = final;
      n.number_of_states = l1.number_of_states + l2.number_of_states + 2;
      nfa_stack_push(s, n);
    } else {
      free_nfa_stack(s);
      return NULL;
    }
  }

  if (nfa_stack_is_empty(s)) {
    free_nfa_stack(s);
    return NULL;
  }

  nfa *n = (nfa *)malloc(sizeof(nfa));
  nfa_stack_pop(s, n);

  if (!nfa_stack_is_empty(s)) {
    free(n);
    free_nfa_stack(s);
    return NULL;
  }

  free_nfa_stack(s);
  return n;
}

int append_to_closures(nfa_state *closure_state, nfa_state ***closures,
                       int *len, int *max) {
  if ((*max) - 1 <= (*len)) {
    *max *= 2;
    nfa_state **temp =
        (nfa_state **)realloc(*closures, sizeof(nfa_state *) * (*max));

    if (temp == NULL)
      return -1;

    *closures = temp;
  }

  (*closures)[(*len)++] = closure_state;
  return 0;
}

nfa_state **get_epsilon_transitions(nfa_state *s, int *transitions_len) {
  int i = 0;
  nfa_state **transitions = (nfa_state **)malloc(sizeof(nfa_state *) * 2);

  if (transitions == NULL)
    return NULL;

  if (s->epsilon != NULL) {
    transitions[i++] = s->epsilon;
  }

  if (s->symbol == '\0' && s->next != NULL) {
    transitions[i++] = s->next;
  }

  *transitions_len = i;
  return transitions;
}

nfa_state **find_epsilon_closures(nfa *n, nfa_state *s, int *closures_len) {
  nfa_state_stack *state_stack = new_nfa_state_stack(n->number_of_states * 2);

  if (state_stack == NULL)
    return NULL;

  int len = 0;
  int max_closures = 5;
  nfa_state **closures =
      (nfa_state **)malloc(sizeof(nfa_state *) * max_closures);

  if (closures == NULL) {
    free_nfa_state_stack(state_stack);
    return NULL;
  }

  nfa_state *prev = NULL;
  nfa_state *curr = s;
  int curr_epsilon_transitions_len;

  int err = 0;
  nfa_state **epsilons = NULL;

  while (curr != NULL) {
    epsilons = get_epsilon_transitions(curr, &curr_epsilon_transitions_len);

    if (epsilons == NULL) {
      free_nfa_state_stack(state_stack);
      free(closures);
      free(epsilons);
      return NULL;
    }

    if (curr->id == n->final->id ||
        (curr->symbol != '\0' && curr->next != NULL)) {

      int is_state_already_exists_in_closures = 0;

      for (int i = 0; i < len; i++) {
        if (closures[i]->id == curr->id) {
          is_state_already_exists_in_closures = 1;
        }
      }

      if (!is_state_already_exists_in_closures) {
        if (append_to_closures(curr, &closures, &len, &max_closures) == -1) {
          free_nfa_state_stack(state_stack);
          free(epsilons);
          free(closures);
          return NULL;
        }
      }
    }

    for (int i = 0; i < curr_epsilon_transitions_len; i++) {
      nfa_state *e = epsilons[i];

      if (prev == NULL || e->id != prev->id) {
        err = nfa_state_stack_push(state_stack, curr);
        err = nfa_state_stack_push(state_stack, epsilons[i]);
        if (err == -1) {
          free_nfa_state_stack(state_stack);
          free(closures);
          free(epsilons);
          return NULL;
        }
      }
    }

    if (!nfa_state_stack_is_empty(state_stack)) {
      err = nfa_state_stack_pop(state_stack, &curr);
      err = nfa_state_stack_pop(state_stack, &prev);
      if (err == -1) {
        free_nfa_state_stack(state_stack);
        free(closures);
        free(epsilons);
        return NULL;
      }
    } else {
      break;
    }
  }

  *closures_len = len;
  free_nfa_state_stack(state_stack);
  free(epsilons);

  return closures;
}

nfa_state **find_epsilon_closures_without_final_states(nfa *n, nfa_state *s,
                                                       int *closures_len) {
  nfa_state_stack *state_stack = new_nfa_state_stack(n->number_of_states * 2);

  if (state_stack == NULL)
    return NULL;

  int len = 0;
  int max_closures = 5;
  nfa_state **closures =
      (nfa_state **)malloc(sizeof(nfa_state *) * max_closures);

  if (closures == NULL) {
    free_nfa_state_stack(state_stack);
    return NULL;
  }

  nfa_state *prev = NULL;
  nfa_state *curr = s;
  int curr_epsilon_transitions_len;

  int err = 0;
  nfa_state **epsilons = NULL;

  while (curr != NULL) {
    epsilons = get_epsilon_transitions(curr, &curr_epsilon_transitions_len);

    if (epsilons == NULL) {
      free_nfa_state_stack(state_stack);
      free(closures);
      free(epsilons);
      return NULL;
    }

    if (curr->symbol != '\0' && curr->next != NULL) {

      int is_state_already_exists_in_closures = 0;

      for (int i = 0; i < len; i++) {
        if (closures[i]->id == curr->id) {
          is_state_already_exists_in_closures = 1;
        }
      }

      if (!is_state_already_exists_in_closures) {
        if (append_to_closures(curr, &closures, &len, &max_closures) == -1) {
          free_nfa_state_stack(state_stack);
          free(closures);
          free(epsilons);
          return NULL;
        }
      }
    }

    for (int i = 0; i < curr_epsilon_transitions_len; i++) {
      nfa_state *e = epsilons[i];

      if (prev == NULL || e->id != prev->id) {
        err = nfa_state_stack_push(state_stack, curr);
        err = nfa_state_stack_push(state_stack, epsilons[i]);
        if (err == -1) {
          free_nfa_state_stack(state_stack);
          free(closures);
          free(epsilons);
          return NULL;
        }
      }
    }

    if (!nfa_state_stack_is_empty(state_stack)) {
      err = nfa_state_stack_pop(state_stack, &curr);
      err = nfa_state_stack_pop(state_stack, &prev);
      if (err == -1) {
        free_nfa_state_stack(state_stack);
        free(closures);
        free(epsilons);
        return NULL;
      }
    } else {
      break;
    }
  }

  *closures_len = len;
  free_nfa_state_stack(state_stack);
  free(epsilons);

  return closures;
}

void free_nfa_state(nfa_state *s, int *visited) {
  if (visited[s->id] != 0) {
    return;
  }

  visited[s->id] = 1;

  if (s->next != NULL && s->epsilon != NULL && s->epsilon->id != s->next->id) {
    free(s->next);
    free(s->epsilon);
  } else if (s->next != NULL) {
    free(s->next);
  } else if (s->epsilon != NULL) {
    free(s->epsilon);
  } else {
    return free(s);
  }

  free(s);
}

void free_nfa(nfa *n) {
  int *visited = (int *)malloc(sizeof(int) * n->number_of_states);

  for (int i = 0; i < n->number_of_states; i++)
    visited[i] = 0;

  free_nfa_state(n->init, visited);
  free(n);
  free(visited);
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

  if (visited == NULL) {
    printf("ERROR ON PRINTING NFA\n");
    return;
  }

  for (int i = 0; i < nfa->number_of_states; i++)
    visited[i] = 0;

  print_nfa_state(nfa->init, visited);
  free(visited);
}
