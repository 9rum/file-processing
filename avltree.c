/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * avltree.c
 * AVL tree implementation
 */

#include "avltree.h"
#include "stack.h"

static inline int max(const int a, const int b) { return a < b ? b : a; }

/**
 * getNode returns a new node.
 */
static inline Node *getNode() {
  Node *node      = malloc(sizeof(Node));
  node -> height  = 1;
  node -> bf      = 0;
  node -> left    = node -> right = NULL;
  return node;
}

/**
 * height returns the height of T.
 * @param T: an AVL tree
 */
static inline int height(const Tree T) { return T == NULL ? 0 : T -> height; }

/**
 * rotateLL implements LL rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
static inline void rotateLL(Tree *T, Node *x, Node *f) {
  Node *l     = x -> left;
  x -> left   = l -> right;
  l -> right  = x;
  if      (f == NULL)       *T          = l;  /* case of root */
  else if (f -> left == x)  f -> left   = l;
  else                      f -> right  = l;

  Node *p     = *T;
  stack stack = NULL;

  while (p != x -> left && p != x -> right) {
    push(&stack, p);
    p = x -> key < p -> key ? p -> left : p -> right;
  }

  while (!empty(stack)) {
    p           = pop(&stack);
    p -> height = 1 + max(height(p -> left), height(p -> right));
    p -> bf     = height(p -> left) - height(p -> right);
  }
}

/**
 * rotateRR implements RR rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
static inline void rotateRR(Tree *T, Node *x, Node *f) {
  Node *r     = x -> right;
  x -> right  = r -> left;
  r -> left   = x;
  if      (f == NULL)       *T          = r;  /*case of root */
  else if (f -> left == x)  f -> left   = r;
  else                      f -> right  = r;

  Node *p     = *T;
  stack stack = NULL;

  while (p != x -> left && p != x -> right) {
    push(&stack, p);
    p = x -> key < p -> key ? p -> left : p -> right;
  }

  while (!empty(stack)) {
    p           = pop(&stack);
    p -> height = 1 + max(height(p -> left), height(p -> right));
    p -> bf     = height(p -> left) - height(p -> right);
  }
}

/**
 * rotateLR implements LR rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
static inline void rotateLR(Tree *T, Node *x, Node *f) {
  Node *l     = x -> left,
       *r     = l -> right;
  l -> right  = r -> left;
  x -> left   = r -> right;
  r -> left   = l;
  r -> right  = x;
  if      (f == NULL)       *T          = r;  /* case of root */
  else if (f -> left == x)  f -> left   = r;
  else                      f -> right  = r;

  Node *p     = *T;
  stack stack = NULL;

  while (p != x -> left && p != x -> right) {
    push(&stack, p);
    p = x -> key < p -> key ? p -> left : p -> right;
  }
  push(&stack, l);

  while (!empty(stack)) {
    p           = pop(&stack);
    p -> height = 1 + max(height(p -> left), height(p ->right));
    p -> bf     = height(p -> left) - height(p -> right);
  }
}

/**
 * rotateRL implements RL rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
static inline void rotateRL(Tree *T, Node *x, Node *f) {
  Node *r     = x -> right,
       *l     = r -> left;
  x -> right  = l -> left;
  r -> left   = l -> right;
  l -> left   = x;
  l -> right  = r;
  if      (f == NULL)       *T          = l;  /* case of root */
  else if (f -> left == x)  f -> left   = l;
  else                      f -> right  = l;

  Node *p     = *T;
  stack stack = NULL;

  while (p != x -> left && p != x -> right) {
    push(&stack, p);
    p = x -> key < p -> key ? p -> left : p -> right;
  }
  push(&stack, r);

  while (!empty(stack)) {
    p           = pop(&stack);
    p -> height = 1 + max(height(p -> left), height(p -> right));
    p -> bf     = height(p -> left) - height(p -> right);
  }
}

/**
 * insertAVL inserts newKey into T.
 * @param T: an AVL tree
 * @param newKey: a key to insert
 */
void insertAVL(Tree *T, const int newKey) {
  Node *p     = *T,
       *q     = NULL,
       *x     = NULL,
       *f     = NULL;
  stack stack = NULL;

  push(&stack, NULL);

  while (p != NULL) {                               /* Phase 1: find position q to insert newKey */
    if  (newKey == p -> key) { clear(&stack); return; }
    push(&stack, p);
    q = p;
    p = newKey < p -> key ? p -> left : p -> right;
  }

  Node *y   = getNode();                            /* Phase 2: insert newKey as a child of q and rebalance */
  y -> key  = newKey;

  if      (*T == NULL)        *T          = y;
  else if (newKey < q -> key) q -> left   = y;
  else                        q -> right  = y;

  while (top(stack) != NULL) {
    p           = pop(&stack);
    p -> height = 1 + max(height(p -> left), height(p -> right));
    p -> bf     = height(p -> left) - height(p -> right);
    if (x == NULL && (1 < p -> bf || p -> bf < -1)) { x = p; f = top(stack); }
  }

  clear(&stack);

  if (x == NULL) return;

  if    (1 < x -> bf) {
    if  (x -> left -> bf < 0)   rotateLR(T, x, f);  /* case of LR */
    else                        rotateLL(T, x, f);  /* case of LL */
  } else {
    if  (0 < x -> right -> bf)  rotateRL(T, x, f);  /* case of RL */
    else                        rotateRR(T, x, f);  /* case of RR */
  }
}

/**
 * deleteAVL deletes deleteKey from T.
 * @param T: an AVL tree
 * @param deleteKey: a key to delete
 */
void deleteAVL(Tree *T, const int deleteKey) {
  Node *p     = *T,
       *q     = NULL,
       *x     = NULL,
       *f     = NULL;
  stack stack = NULL;

  push(&stack, NULL);

  while (p != NULL && deleteKey != p -> key) {
    push(&stack, p);
    q = p;
    p = deleteKey < p -> key ? p -> left : p -> right;
  }

  if (p == NULL) { clear(&stack); return; }

  if (p -> left != NULL && p -> right != NULL) {              /* case of degree 2 */
    push(&stack, p);
    q = p;

    if (p -> bf <= 0) for (p = p -> right; p -> left != NULL; p = p -> left)  push(&stack, p);
    else              for (p = p -> left; p -> right != NULL; p = p -> right) push(&stack, p);

    q -> key  = p -> key;
    q         = top(stack);
  }

  if        (p -> left == NULL && p -> right == NULL) {       /* case of degree 0 */
    if      (q == NULL)       *T          = NULL;             /* case of root */
    else if (q -> left == p)  q -> left   = NULL;
    else                      q -> right  = NULL;
  } else {                                                    /* case of degree 1 */
    if        (p -> left != NULL) {
      if      (q == NULL)       *T          = (*T) -> left;   /* case of root */
      else if (q -> left == p)  q -> left   = p -> left;
      else                      q -> right  = p -> left;
    } else {
      if      (q == NULL)       *T          = (*T) -> right;  /* case of root */
      else if (q -> left == p)  q -> left   = p -> right;
      else                      q -> right  = p -> right;
    }
  }

  free(p);

  while (top(stack) != NULL) {
    p           = pop(&stack);
    p -> height = 1 + max(height(p -> left), height(p -> right));
    p -> bf     = height(p -> left) - height(p -> right);
    if (x == NULL && (1 < p -> bf || p -> bf < -1)) { x = p; f = top(stack); }
  }

  clear(&stack);

  if (x == NULL) return;

  if    (1 < x -> bf) {
    if  (x -> left -> bf < 0)   rotateLR(T, x, f);            /* case of LR */
    else                        rotateLL(T, x, f);            /* case of LL */
  } else {
    if  (0 < x -> right -> bf)  rotateRL(T, x, f);            /* case of RL */
    else                        rotateRR(T, x, f);            /* case of RR */
  }
}

/**
 * inorderAVL implements inorder traversal in T.
 * @param T: an AVL tree
 */
void inorderAVL(const Tree T) { if (T != NULL) { inorderAVL(T -> left); printf("%d ", T -> key); inorderAVL(T -> right); } }
