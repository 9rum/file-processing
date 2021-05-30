/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 *
 * stack.h - generic stack implementation
 *
 * The stack gives the programmer the functionality of a stack
 * - specifically, a LIFO (last-in, first-out) data structure.
 *
 * The stack pushes and pops the element from the top of the stack.
 */
#ifndef _STACK_H
#define _STACK_H

#include <stdlib.h>
#include <stdbool.h>

/**
 * struct stack - generic stack
 *
 * @value: the value of the element
 * @next:  the pointer to the next element
 *
 * See https://en.cppreference.com/w/cpp/container/stack
 */
struct stack {
  void         *value;
  struct stack *next;
};

/**
 * empty - checks whether @stack is empty
 *
 * @stack: stack to check
 */
extern inline bool empty(const struct stack *stack) { return stack == NULL; }

/**
 * top - accesses the top element
 *
 * @stack: stack to access the top element
 */
extern inline void *top(const struct stack *stack) { return empty(stack) ? NULL : stack->value; }

/**
 * push - inserts element at the top
 *
 * @stack: stack to insert element
 * @value: the value of the element to push
 */
extern inline void push(struct stack **stack, void *value) {
  struct stack *top = malloc(sizeof(struct stack));
  top->value        = value;
  top->next         = *stack;
  *stack            = top;
}

/**
 * pop - removes the top element
 *
 * @stack: stack to remove the top element
 */
extern inline void *pop(struct stack **stack) {
  if (empty(*stack))  return NULL;
  struct stack *top   = *stack;
  void         *value = top->value;
  *stack              = top->next;
  free(top);
  return value;
}

/**
 * destroy - empties @stack
 *
 * @stack: stack to empty
 */
extern inline void destroy(struct stack **stack) {
  register struct stack *top;

  while (!empty(*stack)) {
    top    = *stack;
    *stack = top->next;
    free(top);
  }
}

#endif /* _STACK_H */
