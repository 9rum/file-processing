/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * btree.c
 * B-tree implementation
 */

#include "stack.h"
#include "btree.h"

/**
 * getNode returns a new node.
 * @param m: fanout of B-tree
 */
static inline Node *getNode(const unsigned int m) {
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
static inline unsigned int binarySearch(const int *K, const unsigned int n, const int key) {
  register int  i = 0,
                j = n-1;
  register unsigned int mid;

  while (i <= j) {
    mid = i+j>>1;
    if (key == K[mid])  return mid;
    if (key < K[mid])   j = mid-1;
    else                i = mid+1;
  }

  return i;
}

/**
 * insertBT inserts newKey into T.
 * @param T: a B-tree
 * @param m: fanout of B-tree
 * @param newKey: a key to insert
 */
void insertBT(Tree *T, const unsigned int m, const int newKey) {
  register Node *tempNode,
                *x  = *T,
                *y  = NULL;
  stack stack       = NULL,
        iStack      = NULL;
  register int key  = newKey;
  register unsigned int i;

  while (x != NULL) {         /* find position to insert newKey while storing x on the stack */
    if  ((i = binarySearch(x -> K, x -> n, newKey)) < x -> n && newKey == x -> K[i]) { clear(&stack); clear(&iStack); return; }
    push(&stack, x);
    push(&iStack, i);
    x = x -> P[i];
  }

  while (!empty(stack)) {
    x = pop(&stack);
    i = pop(&iStack);

    if (x -> n < m-1) {
      memcpy(&x -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
      x -> K[i] = key;
      if (y != NULL) { memcpy(&x -> P[i+2], &x -> P[i+1], sizeof(Node *)*(x -> n-i)); x -> P[i+1] = y; }
      x -> n++;
      clear(&stack);
      clear(&iStack);
      return;
    }

    tempNode = getNode(m+1);
    memcpy(tempNode -> K, x -> K, sizeof(int)*i);
    memcpy(&tempNode -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
    memcpy(tempNode -> P, x -> P, sizeof(Node *)*(i+1));
    memcpy(&tempNode -> P[i+2], &x -> P[i+1], sizeof(Node *)*(x -> n-i));
    tempNode -> K[i]    = key;
    tempNode -> P[i+1]  = y;

    y = getNode(m);
    memcpy(x -> K, tempNode -> K, sizeof(int)*(m>>1));
    memcpy(x -> P, tempNode -> P, sizeof(Node *)*((m>>1)+1));
    memcpy(y -> K, &tempNode -> K[(m>>1)+1], sizeof(int)*(m-(m>>1)-1));
    memcpy(y -> P, &tempNode -> P[(m>>1)+1], sizeof(Node *)*(m-(m>>1)));
    x -> n  = m>>1;
    y -> n  = m-(m>>1)-1;
    key     = tempNode -> K[m>>1];

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
void deleteBT(Tree *T, const unsigned int m, const int oldKey) {
  register Node *bestSibling,
                *y,
                *x  = *T;
  stack stack       = NULL,
        iStack      = NULL;
  register unsigned int i,
                        b;

  while (x != NULL) {                                         /* find position of oldKey while storing x on the stack */
    i = binarySearch(x -> K, x -> n, oldKey);
    push(&stack, x);
    push(&iStack, i);
    if (i < x -> n && oldKey == x -> K[i]) break;
    x = x -> P[i];
  }

  if (x == NULL) { clear(&stack); clear(&iStack); return; }

  Node *internalNode  = pop(&stack);
  i                   = pop(&iStack);

  if (x -> P[i+1] != NULL) {                                  /* found in internal node */
    push(&stack, x);
    push(&iStack, i+1);
    x = x -> P[i+1];

    while (x != NULL) {
      push(&stack, x);
      push(&iStack, 0);
      x = x -> P[0];
    }
  }

  if (x == NULL) {                                            /* exchange oldKey and the subsequent key */
    x                     = pop(&stack);
    internalNode -> K[i]  = x -> K[0];
    x -> K[0]             = oldKey;
    i                     = pop(&iStack);
  }

  x -> n--;
  memcpy(&x -> K[i], &x -> K[i+1], sizeof(int)*(x -> n-i));

  while (!empty(stack)) {
    if  (m-1>>1 <= x -> n) { clear(&stack); clear(&iStack); return; }

    y           = pop(&stack);
    i           = pop(&iStack);
    b           = i == 0                              ? i+1
                : i == y -> n                         ? i-1
                : y -> P[i-1] -> n < y -> P[i+1] -> n ? i+1
                                                      : i-1;  /* choose bestSibling of x node */
    bestSibling = y -> P[b];

    if    (m-1>>1 < bestSibling -> n) {                       /* case of key redistribution */
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
    if (b < i) {                                              /* case of node merge */
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

  if (x -> n == 0) { *T = x -> P[0]; free(x); }               /* the level of the tree decreases */

  clear(&stack);
  clear(&iStack);
}

/**
 * inorderBT implements inorder traversal in T.
 * @param T: a B-tree
 */
void inorderBT(const Tree T) { if (T != NULL) { for (unsigned int i=0; i<T -> n; ++i) { inorderBT(T -> P[i]); printf("%d ", T -> K[i]); } inorderBT(T -> P[T -> n]); } }
