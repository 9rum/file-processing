/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * stack.c
 * Stack implementation
 */

#include "stack.h"

/**
 * empty checks whether stack is empty.
 * @param stack: a stack
 */
bool empty(const stack stack) { return stack == NULL; }

/**
 * top accesses the top element.
 * @param stack: a stack
 */
void *top(const stack stack) { return empty(stack) ? NULL : stack -> value; }

/**
 * push inserts element at the top.
 * @param stack: a stack
 * @param value: the value of the element to push
 */
void push(stack *stack, void *value) {
  struct stack *top = malloc(sizeof(struct stack));
  top -> value      = value;
  top -> next       = *stack;
  *stack            = top;
}

/**
 * pop removes the top element.
 * @param stack: a stack
 */
void *pop(stack *stack) {
  if (empty(*stack))  return NULL;
  struct stack *top = *stack;
  void *value       = top -> value;
  *stack            = top -> next;
  free(top);
  return value;
}

/**
 * clear empties stack.
 * @param stack: a stack
 */
void clear(stack *stack) { while (!empty(*stack)) pop(stack); }
