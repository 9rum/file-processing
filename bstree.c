/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * bstree.c
 * Binary search tree implementation
 */

#include "stack.h"
#include "bstree.h"

static inline size_t max(const size_t a, const size_t b) { return a < b ? b : a; }

/**
 * getNode returns a new node.
 */
static inline Node *getNode() {
  Node *node      = malloc(sizeof(Node));
  node -> height  = 1;
  node -> left    = node -> right = NULL;
  return node;
}

/**
 * height returns the height of T.
 * @param T: a binary search tree
 */
static inline size_t height(const Tree T) { return T == NULL ? 0 : T -> height; }

/**
 * insertBST inserts newKey into T.
 * @param T: a binary search tree
 * @param newKey: a key to insert
 */
void insertBST(Tree *T, const int newKey) {
  register Node *p  = *T,
                *q  = NULL;
  stack stack       = NULL;

  while (p != NULL) {                                 /* step 1: find position q to insert newKey */
    if  (newKey == p -> key) { clear(&stack); return; }
    push(&stack, p);
    q = p;
    p = newKey < p -> key ? p -> left : p -> right;
  }

  Node *newNode   = getNode();                        /* step 2: make a node with newKey */
  newNode -> key  = newKey;

  if      (*T == NULL)        *T          = newNode;  /* step 3: insert newKey as a child of q */
  else if (newKey < q -> key) q -> left   = newNode;
  else                        q -> right  = newNode;

  while (!empty(stack)) { p = pop(&stack); p -> height = 1 + max(height(p -> left), height(p -> right)); }
}

/**
 * deleteBST deletes deleteKey from T.
 * @param T: a binary search tree
 * @param deleteKey: a key to delete
 */
void deleteBST(Tree *T, const int deleteKey) {
  register Node *p  = *T,
                *q  = NULL;
  stack stack       = NULL;

  while (p != NULL && deleteKey != p -> key) {
    push(&stack, p);
    q = p;
    p = deleteKey < p -> key ? p -> left : p -> right;
  }

  if (p == NULL) { clear(&stack); return; }

  if (p -> left != NULL && p -> right != NULL) {              /* case of degree 2 */
    push(&stack, p);
    q = p;

    if (p -> left -> height <= p -> right -> height)  for (p = p -> right; p -> left != NULL; p = p -> left)  push(&stack, p);
    else                                              for (p = p -> left; p -> right != NULL; p = p -> right) push(&stack, p);

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

  while (!empty(stack)) { p = pop(&stack); p -> height = 1 + max(height(p -> left), height(p -> right)); }
}

/**
 * inorderBST implements inorder traversal in T.
 * @param T: a binary search tree
 */
void inorderBST(const Tree T) { if (T != NULL) { inorderBST(T -> left); printf("%d ", T -> key); inorderBST(T -> right); } }
