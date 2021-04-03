/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * stack.h
 * Stack implementation
 */

#ifndef _STACK_H
#define _STACK_H

#include <stdlib.h>
#include <stdbool.h>

/**
 * stack represents a stack.
 * @see https://en.cppreference.com/w/cpp/container/stack
 */
typedef struct stack {
  void          *value;
  struct stack  *next;
} *stack;

/**
 * empty checks whether stack is empty.
 * @param stack: a stack
 */
bool empty(const stack stack);

/**
 * top accesses the top element.
 * @param stack: a stack
 */
void *top(const stack stack);

/**
 * push inserts element at the top.
 * @param stack: a stack
 * @param value: the value of the element to push
 */
void push(stack *stack, void *value);

/**
 * pop removes the top element.
 * @param stack: a stack
 */
void *pop(stack *stack);

/**
 * clear empties stack.
 * @param stack: a stack
 */
void clear(stack *stack);

#endif /* _STACK_H */
