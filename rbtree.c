/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * rbtree.c
 * Red-black tree implementation
 */

#include "rbtree.h"
#include "stack.h"

/**
 * getNode returns a new node.
 */
static inline Node *getNode() {
  Node *node    = malloc(sizeof(Node));
  node -> color = RED;
  node -> left  = node -> right = NULL;
  return node;
}

/**
 * insertRB inserts newKey into T.
 * @param T: a Red-black tree
 * @param newKey: a key to insert
 */
void insertRB(Tree *T, const int newKey) {
  Node *p     = *T;
  stack stack = NULL;

  while (p != NULL) {
    if  (newKey == p -> key) { clear(&stack); return; }
    push(&stack, p);
    p = newKey < p -> key ? p -> left : p -> right;
  }

  Node *n   = getNode();
  n -> key  = newKey;

  if (*T == NULL) { n -> color = BLACK; *T = n; return; }

  p = top(stack);
  if (newKey < p -> key)  p -> left   = n;
  else                    p -> right  = n;

  while (!empty(stack)) {
    p = pop(&stack);

    if (p -> color == BLACK) break;

    Node *g = pop(&stack);
    Node *u = p == g -> right ? g -> left : g -> right;

    if      (u == NULL || u -> color == BLACK) {  /* case of restructuring */
      if    (g -> left == p) {
        if  (p -> left == n) {
          g -> left   = p -> right;
          p -> right  = g;
          p -> color  = BLACK;
          if (!empty(stack)) {
            Node *f = top(stack);
            if (f -> left == g) f -> left   = p;
            else                f -> right  = p;
          } else                *T          = p;
        } else {
          p -> right  = n -> left;
          g -> left   = n -> right;
          n -> left   = p;
          n -> right  = g;
          n -> color  = BLACK;
          if (!empty(stack)) {
            Node *f = top(stack);
            if (f -> left == g) f -> left   = n;
            else                f -> right  = n;
          } else                *T          = n;
        }
      } else {
        if (p -> left == n) {
          p -> left   = n -> right;
          g -> right  = n -> left;
          n -> left   = g;
          n -> right  = p;
          n -> color  = BLACK;
          if (!empty(stack)) {
            Node *f = top(stack);
            if (f -> left == g) f -> left   = n;
            else                f -> right  = n;
          } else                *T          = n;
        } else {
          g -> right  = p -> left;
          p -> left   = g;
          p -> color  = BLACK;
          if (!empty(stack)) {
            Node *f = top(stack);
            if (f -> left == g) f -> left   = p;
            else                f -> right  = p;
          } else                *T          = p;
        }
      }

      g -> color = RED;
      break;
    }

    if (g != *T) g -> color = RED;                /* case of recoloring */

    p -> color  = BLACK;
    u -> color  = BLACK;
    n           = g;
  }

  clear(&stack);
}

/**
 * deleteRB deletes deleteKey from T.
 * @param T: a Red-black tree
 * @param deleteKey: a key to delete
 */
void deleteRB(Tree *T, const int deleteKey) {
  Node *p     = *T,
       *q     = NULL;
  stack stack = NULL;

  while (p != NULL && deleteKey != p -> key) {
    push(&stack, p);
    q = p;
    p = deleteKey < p -> key ? p -> left : p -> right;
  }

  if (p == NULL) { clear(&stack); return; }

  if (p -> left != NULL && p -> right != NULL) {                                        /* case of degree 2 */
    push(&stack, p);
    q = p;

    for (p = p -> right; p -> left != NULL; p = p -> left) push(&stack, p);

    q -> key  = p -> key;
    q         = top(stack);
  }

  if        (p -> left == NULL && p -> right == NULL) {                                 /* case of degree 0 */
    if      (q == NULL)       *T          = NULL;                                       /* case of root */
    else if (q -> left == p)  q -> left   = NULL;
    else                      q -> right  = NULL;
  } else {                                                                              /* case of degree 1 */
    if        (p -> left != NULL) {
      if      (q == NULL)       *T          = (*T) -> left;                             /* case of root */
      else if (q -> left == p)  q -> left   = p -> left;
      else                      q -> right  = p -> left;
    } else {
      if      (q == NULL)       *T          = (*T) -> right;                            /* case of root */
      else if (q -> left == p)  q -> left   = p -> right;
      else                      q -> right  = p -> right;
    }
  }

  if (p -> color == RED) { free(p); clear(&stack); return; }

  Node *x = p -> right == NULL ? p -> left : p -> right;
  free(p);

  if (x != NULL && x -> color == RED) { x -> color = BLACK; clear(&stack); return; }

  while (!empty(stack)) {                                                               /* case of double black */
    p       = pop(&stack);
    Node *s = x == p -> right ? p -> left : p -> right;

    if (s -> color == RED) {
      p -> color = RED;
      s -> color = BLACK;

      if (p -> left == x) {
        p -> right  = s -> left;
        s -> left   = p;
        if (!empty(stack)) {
          Node *f = top(stack);
          if (f -> left == p) f -> left   = s;
          else                f -> right  = s;
        } else                *T          = s;
      } else {
        p -> left   = s -> right;
        s -> right  = p;
        if (!empty(stack)) {
          Node *f = top(stack);
          if (f -> left == p) f -> left   = s;
          else                f -> right  = s;
        } else                *T          = s;
      }

      push(&stack, s);
      s = x == p -> right ? p -> left : p -> right;
    }

    if    ((s -> left != NULL && s -> left -> color == RED) || (s -> right != NULL && s -> right -> color == RED)) {
      if  (p -> left == s && s -> right != NULL && s -> right -> color == RED) {        /* case of Left Right */
        s -> color          = RED;
        s -> right -> color = BLACK;
        Node *r             = s -> right;
        s -> right          = r -> left;
        r -> left           = s;
        p -> left           = r;
        s                   = p -> left;
      } else if (p -> right == s && s -> left != NULL && s -> left -> color == RED) {   /* case of Right Left */
        s -> color          = RED;
        s -> left -> color  = BLACK;
        Node *r             = s -> left;
        s -> left           = r -> right;
        r -> right          = s;
        p -> right          = r;
        s                   = p -> right;
      }
      if (p -> left == s && s -> left != NULL && s -> left -> color == RED) {           /* case of Left Left */
        s -> color          = p -> color;
        p -> color          = BLACK;
        s -> left -> color  = BLACK;
        p -> left           = s -> right;
        s -> right          = p;
      } else if (p -> right == s && s -> right != NULL && s -> right -> color == RED) { /* case of Right Right */
        s -> color          = p -> color;
        p -> color          = BLACK;
        s -> right -> color = BLACK;
        p -> right          = s -> left;
        s -> left           = p;
      }
      if (!empty(stack)) {
        Node *f = top(stack);
        if (f -> left == p) f -> left   = s;
        else                f -> right  = s;
      } else                *T          = s;

      break;
    }

    if ((s -> left == NULL || s -> left -> color == BLACK) && (s -> right == NULL || s -> right -> color == BLACK)) {
      s -> color = RED;
      if (p -> color == RED) { p -> color = BLACK; break; }
      else x = p;
    }
  }

  clear(&stack);
}

/**
 * inorderRB implements inorder traversal in T.
 * @param T: a Red-black tree
 */
void inorderRB(const Tree T) { if (T != NULL) { inorderRB(T -> left); printf(T -> color == BLACK ? "\033[01;30m%d\033[00m " : "\033[01;31m%d\033[00m ", T -> key); inorderRB(T -> right); } }
