/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * btree.c
 * B-tree implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Node represents a single node in B-tree.
 * @see https://infolab.usc.edu/csci585/Spring2010/den_ar/indexing.pdf
 */
typedef struct Node {
  int         n,  *K;
  struct Node **P;
} Node;

/**
 * getNode returns a new node.
 * @param m: number of branch points in B-tree
 */
Node *getNode(int m) {
  Node *node  = malloc(sizeof(Node));
  node -> n   = 0;
  node -> K   = calloc(m-1, sizeof(int));
  node -> P   = calloc(m, sizeof(Node *));
  return node;
}

typedef Node *Tree;

/**
 * binarySearch returns index i where K[i-1] < key <= K[i].
 * @param K: an array
 * @param n: size of array
 * @param key: a key to search
 */
int binarySearch(int K[], int n, int key) {
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
 * @param m: number of branch points in B-tree
 * @param newKey: a key to insert
 */
void insertBT(Tree *T, int m, int newKey) {
  int cap     = 1,
      size    = 0,
      *iStack = malloc(sizeof(int)*cap);
  Node *x       = *T,
       *y       = NULL,
       **stack  = malloc(sizeof(Node *)*cap);

  while (x != NULL) {         /* find position to insert newKey while storing x on the stack */
    int i = binarySearch(x -> K, x -> n, newKey);
    if (i < x -> n && newKey == x -> K[i]) { free(stack); free(iStack); return; }
    stack[size]   = x;
    iStack[size]  = i;
    x             = x -> P[i];
    if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); iStack = realloc(iStack, sizeof(int)*cap); }
  }

  while (0 <= --size) {
    x     = stack[size];
    int i = iStack[size];

    if (x -> n < m-1) {
      memcpy(&x -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
      x -> K[i] = newKey;
      if (y != NULL) { memcpy(&x -> P[i+2], &x -> P[i+1], sizeof(Node *)*(x -> n-i)); x -> P[i+1] = y; }
      x -> n++;
      free(stack);
      free(iStack);
      return;
    }

    Node *tempNode = getNode(m+1);
    memcpy(tempNode -> K, x -> K, sizeof(int)*i);
    memcpy(&tempNode -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
    memcpy(tempNode -> P, x -> P, sizeof(Node *)*(i+1));
    memcpy(&tempNode -> P[i+2], &x -> P[i+1], sizeof(Node *)*(x -> n-i));
    tempNode -> K[i]    = newKey;
    tempNode -> P[i+1]  = y;

    newKey              = tempNode -> K[m/2];

    memcpy(x -> K, tempNode -> K, sizeof(int)*(m/2));
    memcpy(x -> P, tempNode -> P, sizeof(Node *)*(m/2+1));

    y = getNode(m);
    memcpy(y -> K, &tempNode -> K[m/2+1], sizeof(int)*(m-m/2-1));
    memcpy(y -> P, &tempNode -> P[m/2+1], sizeof(Node *)*(m-m/2));
    x -> n = m/2;
    y -> n = m-m/2-1;

    free(tempNode);
  }

  *T            = getNode(m); /* the level of the tree increases */
  (*T) -> K[0]  = newKey;
  (*T) -> P[0]  = x;
  (*T) -> P[1]  = y;
  (*T) -> n     = 1;
  free(stack);
  free(iStack);
}

/**
 * deleteBT deletes oldKey from T.
 * @param T: a B-tree
 * @param m: number of branch points in B-tree
 * @param oldKey: a key to delete
 */
void deleteBT(Tree *T, int m, int oldKey) {
  int i,
      cap     = 1,
      size    = 0,
      *iStack = malloc(sizeof(int)*cap);
  Node *internalNode,
       *x       = *T,
       **stack  = malloc(sizeof(Node *)*cap);

  while (x != NULL) {                                                                                       /* find position of oldKey while storing x on the stack */
    i = binarySearch(x -> K, x -> n, oldKey);
    if (i < x -> n && oldKey == x -> K[i]) break;
    stack[size]   = x;
    iStack[size]  = i;
    x             = x -> P[i];
    if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); iStack = realloc(iStack, sizeof(int)*cap); }
  }

  if (x == NULL) { free(stack); free(iStack); return; }

  if (x -> P[i+1] != NULL) {                                                                                /* found in internal node */
    internalNode  = x;
    stack[size]   = x;
    iStack[size]  = i+1;
    x             = x -> P[i+1];
    if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); iStack = realloc(iStack, sizeof(int)*cap); }

    while (x != NULL) {
      stack[size]   = x;
      iStack[size]  = 0;
      x             = x -> P[0];
      if (cap <= ++size) { cap <<= 1; stack = realloc(stack, sizeof(Node *)*cap); iStack = realloc(iStack, sizeof(int)*cap); }
    }
  }

  if (x == NULL) {                                                                                          /* exchange oldKey and the subsequent key */
    x                     = stack[--size];
    internalNode -> K[i]  = x -> K[0];
    x -> K[0]             = oldKey;
    i                     = iStack[size];
  }

  x -> n--;
  memcpy(&x -> K[i], &x -> K[i+1], sizeof(int)*(x -> n-i));

  while(0 <= --size) {
    if ((m-1)/2 <= x -> n) { free(stack); free(iStack); return; }

    Node *y           = stack[size];
    i                 = iStack[size];
    int b             = i == 0 ? i+1 : i == y -> n ? i-1 : y -> P[i-1] -> n < y -> P[i+1] -> n ? i+1 : i-1; /* choose bestSibling of x node */
    Node *bestSibling = y -> P[b];

    if ((m-1)/2 < bestSibling -> n) {                                                                       /* case of key redistribution */
      if (b < i) {
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
      x -> n++;
      bestSibling -> n--;
      break;
    }
    if (b < i) {  /* case of node merge */
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

  if (x == *T && x -> n == 0) { *T = x -> P[0]; free(x); }                                                  /* the level of the tree decreases */
  free(stack);
  free(iStack);
}

/**
 * inorderBT implements inorder traversal in T.
 * @param T: a B-tree
 */
void inorderBT(Tree T) { if (T != NULL) { for (int i=0; i<T -> n; i++) { inorderBT(T -> P[i]); printf("%d ", T -> K[i]); } inorderBT(T -> P[T -> n]); } }

int main() {
  int testCases[] = {
    40, 11, 77, 33, 20, 90, 99, 70, 88, 80,
		66, 10, 22, 30, 44, 55, 50, 60, 100, 28,
		18, 9, 5, 17, 6, 3, 1, 4, 2, 7,
		8, 73, 12, 13, 14, 16, 15, 25, 24, 28,
		45, 49, 42, 43, 41, 47, 48, 46, 63, 68,
		61, 62, 64, 69, 67, 65, 54, 59, 58, 51,
		53, 57, 52, 56, 83, 81, 82, 84, 75, 89,
		66, 10, 22, 30, 44, 55, 50, 60, 100, 28,
		18, 9, 5, 17, 6, 3, 1, 4, 2, 7,
		8, 73, 12, 13, 14, 16, 15, 25, 24, 28,
		40, 11, 77, 33, 20, 90, 99, 70, 88, 80,
		45, 49, 42, 43, 41, 47, 48, 46, 63, 68,
		53, 57, 52, 56, 83, 81, 82, 84, 75, 89,
		61, 62, 64, 69, 67, 65, 54, 59, 58, 51,
  };

  Tree T = NULL;

  for (int i=0; i<70; i++)    { insertBT(&T, 3, testCases[i]); inorderBT(T); printf("\n"); }
  for (int i=70; i<140; i++)  { deleteBT(&T, 3, testCases[i]); inorderBT(T); printf("\n"); }
  for (int i=0; i<70; i++)    { insertBT(&T, 4, testCases[i]); inorderBT(T); printf("\n"); }
  for (int i=70; i<140; i++)  { deleteBT(&T, 4, testCases[i]); inorderBT(T); printf("\n"); }
}
