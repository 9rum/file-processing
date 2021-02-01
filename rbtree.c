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

/**
 * getNode returns a new node.
 */
Node *getNode() {
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
void insertRB(Tree *T, int newKey) {
  int cap       = 1,
      size      = 0;
  Node *p       = *T,
       **stack  = malloc(sizeof(Node *)*cap);

  while (p != NULL) {
    if  (newKey == p -> key) { free(stack); return; }
    stack[size] = p;
    p           = newKey < p -> key ? p -> left : p -> right;
    if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); }
  }

  Node *n   = getNode();
  n -> key  = newKey;

  if (*T == NULL) { n -> color = BLACK; *T = n; free(stack); return; }

  p = stack[size-1];
  if (newKey < p -> key)  p -> left   = n;
  else                    p -> right  = n;

  while (0 <= --size) {
    p = stack[size];
    if (p -> color == BLACK) break;

    Node *g = stack[--size];
    Node *u = p == g -> right ? g -> left : g -> right;

    if      (u == NULL || u -> color == BLACK) {  /* case of restructuring */
      if    (g -> left == p) {
        if  (p -> left == n) {
          g -> left   = p -> right;
          p -> right  = g;
          p -> color  = BLACK;
          if    (0 < size) {
            if  (stack[size-1] -> left == g)  stack[size-1] -> left   = p;
            else                              stack[size-1] -> right  = p;
          } else                              *T                      = p;
        } else {
          p -> right  = n -> left;
          g -> left   = n -> right;
          n -> left   = p;
          n -> right  = g;
          n -> color  = BLACK;
          if    (0 < size) {
            if  (stack[size-1] -> left == g)  stack[size-1] -> left   = n;
            else                              stack[size-1] -> right  = n;
          } else                              *T                      = n;
        }
      } else {
        if (p -> left == n) {
          p -> left   = n -> right;
          g -> right  = n -> left;
          n -> left   = g;
          n -> right  = p;
          n -> color  = BLACK;
          if    (0 < size) {
            if  (stack[size-1] -> left == g)  stack[size-1] -> left   = n;
            else                              stack[size-1] -> right  = n;
          } else                              *T                      = n;
        } else {
          g -> right  = p -> left;
          p -> left   = g;
          p -> color  = BLACK;
          if    (0 < size) {
            if  (stack[size-1] -> left == g)  stack[size-1] -> left   = p;
            else                              stack[size-1] -> right  = p;
          } else                              *T                      = p;
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

  free(stack);
}

/**
 * deleteRB deletes deleteKey from T.
 * @param T: a Red-black tree
 * @param deleteKey: a key to delete
 */
void deleteRB(Tree *T, int deleteKey) {
  int cap       = 1,
      size      = 0;
  Node *p       = *T,
       *q       = NULL,
       **stack  = malloc(sizeof(Node *)*cap);

  while (p != NULL && deleteKey != p -> key) {
    q           = p;
    stack[size] = q;
    p           = deleteKey < p -> key ? p -> left : p -> right;
    if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); }
  }

  if (p == NULL) { free(stack); return; }

  if (p -> left != NULL && p -> right != NULL) {                                        /* case of degree 2 */
    stack[size] = p;
    q           = p;
    if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); }

    for (p = p -> right; p -> left != NULL; p = p -> left) { stack[size] = p; if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); } }

    q -> key  = p -> key;
    q         = stack[size-1];
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

  if (p -> color == RED) { free(p); free(stack); return; }

  Node *x = p -> right == NULL ? p -> left : p -> right;
  free(p);

  if (x != NULL && x -> color == RED) { x -> color = BLACK; free(stack); return; }

  while (0 <= --size) {                                                                 /* case of double black */
    p       = stack[size];
    Node *s = x == p -> right ? p -> left : p -> right;
    
    if (s -> color == RED) {
      p -> color = RED;
      s -> color = BLACK;
      
      if (p -> left == x) {
        p -> right  = s -> left;
        s -> left   = p;
        if    (0 < size) {
          if  (stack[size-1] -> left == p)  stack[size-1] -> left   = s;
          else                              stack[size-1] -> right  = s;
        } else                              *T                      = s;
      } else {
        p -> left   = s -> right;
        s -> right  = p;
        if    (0 < size) {
          if  (stack[size-1] -> left == p)  stack[size-1] -> left   = s;
          else                              stack[size-1] -> right  = s;
        } else                              *T                      = s;
      }
      stack[size] = s;
      if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); }
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
        s -> color = p -> color;
        p -> color = BLACK;
        s -> right -> color = BLACK;
        p -> right = s -> left;
        s -> left = p;
      }
      if    (0 < size) {
        if  (stack[size-1] -> left == p)  stack[size-1] -> left   = s;
        else                              stack[size-1] -> right  = s;
      } else                              *T                      = s;
      stack[size] = s;
      if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); }
      break;
    }
    
    
    if ((s -> left == NULL || s -> left -> color == BLACK) && (s -> right == NULL || s -> right -> color == BLACK)) {
      s -> color = RED;
      if (p -> color == RED) { p -> color = BLACK; break; }
      else x = p;
    }
  }

  free(stack);
}

/**
 * inorderRB implements inorder traversal in T.
 * @param T: a Red-black tree
 */
void inorderRB(Tree T) { if (T != NULL) { inorderRB(T -> left); printf(T -> color == BLACK ? "\033[01;30m%d\033[00m " : "\033[01;31m%d\033[00m ", T -> key); inorderRB(T -> right); } }
