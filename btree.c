/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * btree.c
 * B-tree implementation
 */

#include "btree.h"
#include "stack.h"

/**
 * getNode returns a new node.
 * @param m: fanout of B-tree
 */
static inline Node *getNode(const int m) {
  Node *node  = malloc(sizeof(Node));
  node -> n   = 0;
  node -> K   = malloc(sizeof(int)*(m-1));
  node -> P   = calloc(m, sizeof(Node *));
  return node;
}

/**
 * binarySearch returns index i where K[i-1] < key <= K[i].
 * @param K: an array
 * @param n: size of array
 * @param key: a key to search
 */
static inline int binarySearch(const int *K, const int n, const int key) {
  int i = 0,
      j = n-1;

  while (i <= j) {
    int mid = i + (j-i)/2;
    if      (key == K[mid]) return mid;
    else if (key < K[mid])  j = mid-1;
    else                    i = mid+1;
  }

  return i;
}

/**
 * insertBT inserts newKey into T.
 * @param T: a B-tree
 * @param m: fanout of B-tree
 * @param newKey: a key to insert
 */
void insertBT(Tree *T, const int m, const int newKey) {
  Node *x       = *T,
       *y       = NULL;
  stack stack   = NULL,
        iStack  = NULL;
  int key       = newKey;

  while (x != NULL) {         /* find position to insert newKey while storing x on the stack */
    int i = binarySearch(x -> K, x -> n, newKey);
    if (i < x -> n && newKey == x -> K[i]) { clear(&stack); clear(&iStack); return; }
    push(&stack, x);
    push(&iStack, i);
    x = x -> P[i];
  }

  while (!empty(stack)) {
    x     = pop(&stack);
    int i = pop(&iStack);

    if (x -> n < m-1) {
      memcpy(&x -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
      x -> K[i] = key;
      if (y != NULL) { memcpy(&x -> P[i+2], &x -> P[i+1], sizeof(Node *)*(x -> n-i)); x -> P[i+1] = y; }
      x -> n++;
      clear(&stack);
      clear(&iStack);
      return;
    }

    Node *tempNode = getNode(m+1);
    memcpy(tempNode -> K, x -> K, sizeof(int)*i);
    memcpy(&tempNode -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
    memcpy(tempNode -> P, x -> P, sizeof(Node *)*(i+1));
    memcpy(&tempNode -> P[i+2], &x -> P[i+1], sizeof(Node *)*(x -> n-i));
    tempNode -> K[i]    = key;
    tempNode -> P[i+1]  = y;

    y = getNode(m);
    memcpy(x -> K, tempNode -> K, sizeof(int)*(m/2));
    memcpy(x -> P, tempNode -> P, sizeof(Node *)*(m/2+1));
    memcpy(y -> K, &tempNode -> K[m/2+1], sizeof(int)*(m-m/2-1));
    memcpy(y -> P, &tempNode -> P[m/2+1], sizeof(Node *)*(m-m/2));
    x -> n  = m/2;
    y -> n  = m-m/2-1;
    key     = tempNode -> K[m/2];

    free(tempNode);
  }

  *T            = getNode(m); /* the level of the tree increases */
  (*T) -> K[0]  = key;
  (*T) -> P[0]  = x;
  (*T) -> P[1]  = y;
  (*T) -> n     = 1;

  clear(&stack);
  clear(&iStack);
}

/**
 * deleteBT deletes oldKey from T.
 * @param T: a B-tree
 * @param m: fanout of B-tree
 * @param oldKey: a key to delete
 */
void deleteBT(Tree *T, const int m, const int oldKey) {
  Node *x       = *T;
  stack stack   = NULL,
        iStack  = NULL;

  while (x != NULL) {                                                                                       /* find position of oldKey while storing x on the stack */
    int i = binarySearch(x -> K, x -> n, oldKey);
    push(&stack, x);
    push(&iStack, i);
    if (i < x -> n && oldKey == x -> K[i]) break;
    x = x -> P[i];
  }

  if (x == NULL) { clear(&stack); clear(&iStack); return; }

  Node *internalNode  = pop(&stack);
  int i               = pop(&iStack);

  if (x -> P[i+1] != NULL) {                                                                                /* found in internal node */
    push(&stack, x);
    push(&iStack, i+1);
    x = x -> P[i+1];

    while (x != NULL) {
      push(&stack, x);
      push(&iStack, 0);
      x = x -> P[0];
    }
  }

  if (x == NULL) {                                                                                          /* exchange oldKey and the subsequent key */
    x                     = pop(&stack);
    internalNode -> K[i]  = x -> K[0];
    x -> K[0]             = oldKey;
    i                     = pop(&iStack);
  }

  x -> n--;
  memcpy(&x -> K[i], &x -> K[i+1], sizeof(int)*(x -> n-i));

  while (!empty(stack)) {
    if  ((m-1)/2 <= x -> n) { clear(&stack); clear(&iStack); return; }

    Node *y           = pop(&stack);
    i                 = pop(&iStack);
    int b             = i == 0 ? i+1 : i == y -> n ? i-1 : y -> P[i-1] -> n < y -> P[i+1] -> n ? i+1 : i-1; /* choose bestSibling of x node */
    Node *bestSibling = y -> P[b];

    if    ((m-1)/2 < bestSibling -> n) {                                                                    /* case of key redistribution */
      if  (b < i) {
        memcpy(&x -> K[1], x -> K, sizeof(int)*x -> n);
        memcpy(&x -> P[1], x -> P, sizeof(Node *)*(x -> n+1));
        x -> K[0]   = y -> K[i-1];
        y -> K[i-1] = bestSibling -> K[bestSibling -> n-1];
        x -> P[0]   = bestSibling -> P[bestSibling -> n];
      } else {
        x -> K[x -> n]    = y -> K[i];
        y -> K[i]         = bestSibling -> K[0];
        x -> P[x -> n+1]  = bestSibling -> P[0];
        memcpy(bestSibling -> K, &bestSibling -> K[1], sizeof(int)*(bestSibling -> n-1));
        memcpy(bestSibling -> P, &bestSibling -> P[1], sizeof(Node *)*bestSibling -> n);
      }
      bestSibling -> P[bestSibling -> n] = NULL;
      bestSibling -> n--;
      x -> n++;
      break;
    }
    if (b < i) {                                                                                            /* case of node merge */
      bestSibling -> K[bestSibling -> n] = y -> K[i-1];
      memcpy(&bestSibling -> K[bestSibling -> n+1], x -> K, sizeof(int)*x -> n);
      memcpy(&bestSibling -> P[bestSibling -> n+1], x -> P, sizeof(Node *)*(x -> n+1));
      memcpy(&y -> K[i-1], &y -> K[i], sizeof(int)*(y -> n-i));
      memcpy(&y -> P[i], &y -> P[i+1], sizeof(Node *)*(y -> n-i));
      bestSibling -> n += x -> n+1;
      free(x);
    } else {
      x -> K[x -> n] = y -> K[i];
      memcpy(&x -> K[x -> n+1], bestSibling -> K, sizeof(int)*bestSibling -> n);
      memcpy(&x -> P[x -> n+1], bestSibling -> P, sizeof(Node *)*(bestSibling -> n+1));
      memcpy(&y -> K[i], &y -> K[i+1], sizeof(int)*(y -> n-i-1));
      memcpy(&y -> P[i+1], &y -> P[i+2], sizeof(Node *)*(y -> n-i-1));
      x -> n += bestSibling -> n+1;
      free(bestSibling);
    }
    y -> P[y -> n] = NULL;
    y -> n--;
    x = y;
  }

  if (x -> n == 0) { *T = x -> P[0]; free(x); }                                                             /* the level of the tree decreases */

  clear(&stack);
  clear(&iStack);
}

/**
 * inorderBT implements inorder traversal in T.
 * @param T: a B-tree
 */
void inorderBT(const Tree T) { if (T != NULL) { for (int i=0; i<T -> n; i++) { inorderBT(T -> P[i]); printf("%d ", T -> K[i]); } inorderBT(T -> P[T -> n]); } }
