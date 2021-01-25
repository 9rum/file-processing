/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * avltree.c
 * AVL tree implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include "avltree.h"

extern inline int max(int a, int b) { return a < b ? b : a; }

/**
 * getNode returns a new node.
 */
Node *getNode() {
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
int height(Tree T) { return T == NULL ? 0 : T -> height; }

/**
 * rotateLL implements LL rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
void rotateLL(Tree *T, Node *x, Node *f) {
  Node *l     = x -> left;
  x -> left   = l -> right;
  l -> right  = x;
  if      (f == NULL)       *T          = l;  /* case of root */
  else if (f -> left == x)  f -> left   = l;
  else                      f -> right  = l;
  
  Node **stack  = malloc(sizeof(Node *)*height(*T)),
       *p       = *T;
  int size      = 0;

  while (p != x -> left && p != x -> right) {
    stack[size++] = p;
    p             = x -> key < p -> key ? p -> left : p -> right;
  }
  
  while (0 <= --size) {
    stack[size] -> height = 1 + max(height(stack[size] -> left), height(stack[size] -> right));
    stack[size] -> bf     = height(stack[size] -> left) - height(stack[size] -> right);
  }
  
  free(stack);
}

/**
 * rotateRR implements RR rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
void rotateRR(Tree *T, Node *x, Node *f) {
  Node *r     = x -> right;
  x -> right  = r -> left;
  r -> left   = x;
  if      (f == NULL)       *T          = r;  /*case of root */
  else if (f -> left == x)  f -> left   = r;
  else                      f -> right  = r;

  Node **stack  = malloc(sizeof(Node *)*height(*T)),
       *p       = *T;
  int size      = 0;
  
  while (p != x -> left && p != x -> right) {
    stack[size++] = p;
    p             = x -> key < p -> key ? p -> left : p -> right;
  }
  
  while (0 <= --size) {
    stack[size] -> height = 1 + max(height(stack[size] -> left), height(stack[size] -> right));
    stack[size] -> bf     = height(stack[size] -> left) - height(stack[size] -> right);
  }
  
  free(stack);
}

/**
 * rotateLR implements LR rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
void rotateLR(Tree *T, Node *x, Node *f) {
  Node *l     = x -> left,
       *r     = l -> right;
  l -> right  = r -> left;
  x -> left   = r -> right;
  r -> left   = l;
  r -> right  = x;
  if      (f == NULL)       *T          = r;  /* case of root */
  else if (f -> left == x)  f -> left   = r;
  else                      f -> right  = r;
  
  Node **stack  = malloc(sizeof(Node *)*(height(*T)+1)),
       *p       = *T;
  int size      = 0;
  
  while (p != x -> left && p != x -> right) {
    stack[size++] = p;
    p             = x -> key < p -> key ? p -> left : p -> right;
  }
  stack[size++] = l;
  
  while (0 <= --size) {
    stack[size] -> height = 1 + max(height(stack[size] -> left), height(stack[size] -> right));
    stack[size] -> bf     = height(stack[size] -> left) - height(stack[size] -> right);
  }
  
  free(stack);
}

/**
 * rotateRL implements RL rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
void rotateRL(Tree *T, Node *x, Node *f) {
  Node *r     = x -> right,
       *l     = r -> left;
  x -> right  = l -> left;
  r -> left   = l -> right;
  l -> left   = x;
  l -> right  = r;
  if      (f == NULL)       *T          = l;  /* case of root */
  else if (f -> left == x)  f -> left   = l;
  else                      f -> right  = l;

  Node **stack  = malloc(sizeof(Node *)*(height(*T)+1)),
       *p       = *T;
  int size      = 0;
  
  while (p != x -> left && p != x -> right) {
    stack[size++] = p;
    p             = x -> key < p -> key ? p -> left : p -> right;
  }
  stack[size++] = r;
  
  while (0 <= --size) {
    stack[size] -> height = 1 + max(height(stack[size] -> left), height(stack[size] -> right));
    stack[size] -> bf     = height(stack[size] -> left) - height(stack[size] -> right);
  }
  
  free(stack);
}

/**
 * insertAVL inserts newKey into T.
 * @param T: an AVL tree
 * @param newKey: a key to insert
 */
void insertAVL(Tree *T, int newKey) {
  Node **stack  = malloc(sizeof(Node *)*(height(*T)+1)),
       *p       = *T,
       *q       = NULL,
       *x       = NULL,
       *f       = NULL;
  int size      = 0;
  stack[size++] = NULL;

  while (p != NULL) {                               /* Phase 1: find position q to insert newKey */
    if  (newKey == p -> key) { free(stack); return; }
    q             = p;
    stack[size++] = q;
    p             = newKey < p -> key ? p -> left : p -> right;
  }

  Node *y   = getNode();                            /* Phase 2: insert newKey as a child of q and rebalance */
  y -> key  = newKey;

  if      (*T == NULL)        *T          = y;
  else if (newKey < q -> key) q -> left   = y;
  else                        q -> right  = y;
  
  while (1 <= --size) {
    stack[size] -> height = 1 + max(height(stack[size] -> left), height(stack[size] -> right));
    stack[size] -> bf     = height(stack[size] -> left) - height(stack[size] -> right);
    if (x == NULL && (1 < stack[size] -> bf || stack[size] -> bf < -1)) x = stack[size], f = stack[size-1];
  }
  
  free(stack);

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
void deleteAVL(Tree *T, int deleteKey) {
  Node **stack  = malloc(sizeof(Node *)*(height(*T)+1)),
       *p       = *T,
       *q       = NULL,
       *x       = NULL,
       *f       = NULL;
  int size      = 0;
  stack[size++] = NULL;

  while (p != NULL && deleteKey != p -> key) {
    q             = p;
    stack[size++] = q;
    p             = deleteKey < p -> key ? p -> left : p -> right;
  }

  if (p == NULL) { free(stack); return; }

  if        (p -> left == NULL && p -> right == NULL) {       /* case of degree 0 */
    if      (q == NULL)       *T          = NULL;             /* case of root */
    else if (q -> left == p)  q -> left   = NULL;
    else                      q -> right  = NULL;
  } else if   (p -> left == NULL || p -> right == NULL) {     /* case of degree 1 */
    if        (p -> left != NULL) {
      if      (q == NULL)       *T          = (*T) -> left;   /* case of root */
      else if (q -> left == p)  q -> left   = p -> left;
      else                      q -> right  = p -> left;
    } else {
      if      (q == NULL)       *T          = (*T) -> right;  /* case of root */
      else if (q -> left == p)  q -> left   = p -> right;
      else                      q -> right  = p -> right;
    }
  } else {                                                    /* case of degree 2 */
    stack[size++]   = p;
    Node *tempNode  = p;

    if (p -> bf <= 0) for (p = p -> right; p -> left != NULL; p = p -> left)  stack[size++] = p;
    else              for (p = p -> left; p -> right != NULL; p = p -> right) stack[size++] = p;

    tempNode -> key = p -> key;
    q               = stack[size-1];

    if    (p -> left == NULL && p -> right == NULL) {         /* case of degree 0 */
      if  (q -> left == p)  q -> left   = NULL;
      else                  q -> right  = NULL;
    } else {                                                  /* case of degree 1 */
      if    (p -> left != NULL) {
        if  (q -> left == p)  q -> left   = p -> left;
        else                  q -> right  = p -> left;
      } else {
        if  (q -> left == p)  q -> left   = p -> right;
        else                  q -> right  = p -> right;
      }
    }
  }
  
  while (1 <= --size) {
    stack[size] -> height = 1 + max(height(stack[size] -> left), height(stack[size] -> right));
    stack[size] -> bf     = height(stack[size] -> left) - height(stack[size] -> right);
    if (x == NULL && (1 < stack[size] -> bf || stack[size] -> bf < -1)) { x = stack[size]; f = stack[size-1]; }
  }
  
  free(p);
  free(stack);

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
void inorderAVL(Tree T) { if (T != NULL) { inorderAVL(T -> left); printf("%d ", T -> key); inorderAVL(T -> right); } }
