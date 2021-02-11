/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * bplustree.c
 * B+-tree implementation
 */

#include "bplustree.h"

/**
 * getTerminalNode returns a new terminal node.
 * @param m: fanout of B+-tree
 */
static inline TerminalNode *getTerminalNode(int m) {
  TerminalNode *node  = malloc(sizeof(TerminalNode));
  node -> q           = 0;
  node -> K           = malloc(sizeof(int)*m);
  node -> P           = NULL;
  return node;
}

/**
 * getInternalNode returns a new internal node.
 * @param m: fanout of B+-tree
 */
static inline InternalNode *getInternalNode(int m) {
  InternalNode *node  = malloc(sizeof(InternalNode));
  node -> n           = 0;
  node -> K           = malloc(sizeof(int)*(m-1));
  node -> Pi          = NULL;
  node -> Pt          = NULL;
  return node;
}

/**
 * binarySearch returns index i where K[i-1] < key <= K[i].
 * @param K: an array
 * @param n: size of array
 * @param key: a key to search
 */
static inline int binarySearch(int K[], int n, int key) {
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
 * insertBPT inserts newKey into T.
 * @param T: a B+-tree
 * @param m: fanout of B+-tree
 * @param newKey: a key to insert
 */
void insertBPT(Tree *T, int m, int newKey) {
  if (*T == NULL) {
    *T                  = malloc(sizeof(struct Tree));
    (*T) -> IndexSet    = NULL;
    (*T) -> SequenceSet = NULL;
  }

  int cap               = 1,
      size              = 0,
      *iStack           = malloc(sizeof(int)*cap);
  InternalNode *x       = (*T) -> IndexSet,
               *y       = NULL,
               **stack  = malloc(sizeof(InternalNode *)*cap);
  TerminalNode *z       = (*T) -> SequenceSet;

  while (x != NULL) {                             /* find position to insert newKey while storing x on the stack */
    int i         = binarySearch(x -> K, x -> n, newKey);
    stack[size]   = x;
    iStack[size]  = i;
    if (x -> Pi != NULL)  { x = x -> Pi[i]; }
    else                  { z = x -> Pt[i]; x = NULL; }
    if (cap <= ++size)    { cap <<= 1; stack = realloc(stack, sizeof(InternalNode *)*cap); iStack = realloc(iStack, sizeof(int)*cap); }
  }

  if (z == NULL) {
    (*T) -> SequenceSet         = getTerminalNode(m);
    (*T) -> SequenceSet -> K[0] = newKey;
    (*T) -> SequenceSet -> q++;
    free(stack);
    free(iStack);
    return;
  }

  int i = binarySearch(z -> K, z -> q, newKey);
  if (i < z -> q && newKey == z -> K[i]) { free(stack); free(iStack); return; }

  if (z -> q < m) {
    memcpy(&z -> K[i+1], &z -> K[i], sizeof(int)*(z -> q-i));
    z -> K[i] = newKey;
    z -> q++;
    free(stack);
    free(iStack);
    return;
  }

  TerminalNode *TempNode  = getTerminalNode(m+1),
               *newNode   = getTerminalNode(m);
  memcpy(TempNode -> K, z -> K, sizeof(int)*i);
  memcpy(&TempNode -> K[i+1], &z -> K[i], sizeof(int)*(z -> q-i));
  TempNode -> K[i] = newKey;

  memcpy(z -> K, TempNode -> K, sizeof(int)*((m+1)/2));
  memcpy(newNode -> K, &TempNode -> K[(m+1)/2], sizeof(int)*(m/2+1));
  z -> q        = (m+1)/2;
  newNode -> q  = m/2+1;
  newKey        = z -> K[z -> q-1];
  newNode -> P  = z -> P;
  z -> P        = newNode;

  free(TempNode);

  if (size == 0) {
    (*T) -> IndexSet          = getInternalNode(m);
    (*T) -> IndexSet -> Pt    = calloc(m, sizeof(TerminalNode *));
    (*T) -> IndexSet -> K[0]  = newKey;
    (*T) -> IndexSet -> Pt[0] = z;
    (*T) -> IndexSet -> Pt[1] = newNode;
    (*T) -> IndexSet -> n++;
    free(stack);
    free(iStack);
    return;
  }

  x = stack[--size];
  i = iStack[size];

  if (x -> n < m-1) {
    memcpy(&x -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
    memcpy(&x -> Pt[i+2], &x -> Pt[i+1], sizeof(TerminalNode *)*(x -> n-i));
    x -> K[i]     = newKey;
    x -> Pt[i+1]  = newNode;
    x -> n++;
    free(stack);
    free(iStack);
    return;
  }

  InternalNode *tempNode  = getInternalNode(m+1);
  tempNode -> Pt          = calloc(m+1, sizeof(TerminalNode *));
  memcpy(tempNode -> K, x -> K, sizeof(int)*i);
  memcpy(&tempNode -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
  memcpy(tempNode -> Pt, x -> Pt, sizeof(TerminalNode *)*(i+1));
  memcpy(&tempNode -> Pt[i+2], &x -> Pt[i+1], sizeof(TerminalNode *)*(x -> n-i));
  tempNode -> K[i]    = newKey;
  tempNode -> Pt[i+1] = newNode;

  y       = getInternalNode(m);
  y -> Pt = calloc(m, sizeof(TerminalNode *));
  memcpy(x -> K, tempNode -> K, sizeof(int)*(m/2));
  memcpy(x -> Pt, tempNode -> Pt, sizeof(TerminalNode *)*(m/2+1));
  memcpy(y -> K, &tempNode -> K[m/2+1], sizeof(int)*(m-m/2-1));
  memcpy(y -> Pt, &tempNode -> Pt[m/2+1], sizeof(TerminalNode *)*(m-m/2));
  x -> n = m/2;
  y -> n = m-m/2-1;
  newKey = tempNode -> K[m/2];

  free(tempNode);

  while (0 <= --size) {
    x = stack[size];
    i = iStack[size];

    if (x -> n < m-1) {
      memcpy(&x -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
      memcpy(&x -> Pi[i+2], &x -> Pi[i+1], sizeof(InternalNode *)*(x -> n-i));
      x -> K[i]     = newKey;
      x -> Pi[i+1]  = y;
      x -> n++;
      free(stack);
      free(iStack);
      return;
    }

    tempNode        = getInternalNode(m+1);
    tempNode -> Pi  = calloc(m+1, sizeof(InternalNode *));
    memcpy(tempNode -> K, x -> K, sizeof(int)*i);
    memcpy(&tempNode -> K[i+1], &x -> K[i], sizeof(int)*(x -> n-i));
    memcpy(tempNode -> Pi, x -> Pi, sizeof(InternalNode *)*(i+1));
    memcpy(&tempNode -> Pi[i+2], &x -> Pi[i+1], sizeof(InternalNode *)*(x -> n-i));
    tempNode -> K[i]    = newKey;
    tempNode -> Pi[i+1] = y;

    y       = getInternalNode(m);
    y -> Pi = calloc(m, sizeof(InternalNode *));
    memcpy(x -> K, tempNode -> K, sizeof(int)*(m/2));
    memcpy(x -> Pi, tempNode -> Pi, sizeof(InternalNode *)*(m/2+1));
    memcpy(y -> K, &tempNode -> K[m/2+1], sizeof(int)*(m-m/2-1));
    memcpy(y -> Pi, &tempNode -> Pi[m/2+1], sizeof(InternalNode *)*(m-m/2));
    x -> n = m/2;
    y -> n = m-m/2-1;
    newKey = tempNode -> K[m/2];

    free(tempNode);
  }

  (*T) -> IndexSet          = getInternalNode(m); /* the level of tree increases */
  (*T) -> IndexSet -> Pi    = calloc(m, sizeof(InternalNode *));
  (*T) -> IndexSet -> K[0]  = newKey;
  (*T) -> IndexSet -> Pi[0] = x;
  (*T) -> IndexSet -> Pi[1] = y;
  (*T) -> IndexSet -> n     = 1;

  free(stack);
  free(iStack);
}

/**
 * deleteBPT deletes oldKey from T.
 * @param T: a B+-tree
 * @param m: fanout of B+-tree
 * @param oldKey: a key to delete
 */
void deleteBPT(Tree *T, int m, int oldKey) {
  if (*T == NULL) return;

  int cap               = 1,
      size              = 0,
      *iStack           = malloc(sizeof(int)*cap);
  InternalNode *x       = (*T) -> IndexSet,
               *y       = NULL,
               **stack  = malloc(sizeof(InternalNode *)*cap);
  TerminalNode *z       = (*T) -> SequenceSet;

  while (x != NULL) {                                                                                               /* find position of oldKey while storing x on the stack */
    int i         = binarySearch(x -> K, x -> n, oldKey);
    stack[size]   = x;
    iStack[size]  = i;
    if (x -> Pi != NULL)  { x = x -> Pi[i]; }
    else                  { z = x -> Pt[i]; x = NULL; }
    if (cap <= ++size)    { cap <<= 1; stack = realloc(stack, sizeof(InternalNode *)*cap); iStack = realloc(iStack, sizeof(int)*cap); }
  }

  int i = binarySearch(z -> K, z -> q, oldKey);
  if (i < z -> q && oldKey != z -> K[i] || z -> q <= i) { free(stack); free(iStack); return; }

  z -> q--;
  memcpy(&z -> K[i], &z -> K[i+1], sizeof(int)*(z -> q-i));

  if ((m+1)/2 <= z -> q) { free(stack); free(iStack); return; }

  if    (size == 0) {
    if  (z -> q == 0) { (*T) -> SequenceSet = NULL; *T = NULL; free(z); }
    free(stack);
    free(iStack);
    return;
  }

  x                         = stack[--size];
  i                         = iStack[size];
  int b                     = i == 0 ? i+1 : i == x -> n ? i-1 : x -> Pt[i-1] -> q < x -> Pt[i+1] -> q ? i+1 : i-1; /* choose bestSibling of z node */
  TerminalNode *BestSibling = x -> Pt[b];

  if    ((m+1)/2 < BestSibling -> q) {                                                                              /* case of key redistribution */
    if  (b < i) {
      memcpy(&z -> K[1], z -> K, sizeof(int)*z -> q);
      z -> K[0]   = BestSibling -> K[BestSibling -> q-1];
      x -> K[i-1] = BestSibling -> K[BestSibling -> q-2];
    } else {
      z -> K[z -> q]  = BestSibling -> K[0];
      x -> K[i]       = z -> K[z -> q];
      memcpy(BestSibling -> K, &BestSibling -> K[1], sizeof(int)*(BestSibling -> q-1));
    }
    z -> q++;
    BestSibling -> q--;
    free(stack);
    free(iStack);
    return;
  }

  if (b < i) {                                                                                                      /* case of terminal node merge */
    memcpy(&BestSibling -> K[BestSibling -> q], z -> K, sizeof(int)*z -> q);
    memcpy(&x -> K[i-1], &x -> K[i], sizeof(int)*(x -> n-i));
    memcpy(&x -> Pt[i], &x -> Pt[i+1], sizeof(TerminalNode *)*(x -> n-i));
    BestSibling -> q += z -> q;
    BestSibling -> P  = z -> P;
    free(z);
  } else {
    memcpy(&z -> K[z -> q], BestSibling -> K, sizeof(int)*BestSibling -> q);
    memcpy(&x -> K[i], &x -> K[i+1], sizeof(int)*(x -> n-i-1));
    memcpy(&x -> Pt[i+1], &x -> Pt[i+2], sizeof(TerminalNode *)*(x -> n-i-1));
    z -> q += BestSibling -> q;
    z -> P  = BestSibling -> P;
    free(BestSibling);
  }
  x -> Pt[x -> n] = NULL;
  x -> n--;

  if    ((m-1)/2 <= x -> n) { free(stack); free(iStack); return; }
  if    (size == 0) {
    if  (x -> n == 0) { (*T) -> IndexSet = NULL; free(x); }
    free(stack);
    free(iStack);
    return;
  }

  y                         = stack[--size];
  i                         = iStack[size];
  b                         = i == 0 ? i+1 : i == y -> n ? i-1 : y -> Pi[i-1] -> n < y -> Pi[i+1] -> n ? i+1 : i-1; /* choose bestSibling of x node */
  InternalNode *bestSibling = y -> Pi[b];

  if    ((m-1)/2 < bestSibling -> n) {                                                                              /* case of key redistribution */
    if  (b < i) {
      memcpy(&x -> K[1], x -> K, sizeof(int)*x -> n);
      memcpy(&x -> Pt[1], x -> Pt, sizeof(TerminalNode *)*(x -> n+1));
      x -> K[0]   = y -> K[i-1];
      y -> K[i-1] = bestSibling -> K[bestSibling -> n-1];
      x -> Pt[0]  = bestSibling -> Pt[bestSibling -> n];
    } else {
      x -> K[x -> n]    = y -> K[i];
      y -> K[i]         = bestSibling -> K[0];
      x -> Pt[x -> n+1] = bestSibling -> Pt[0];
      memcpy(bestSibling -> K, &bestSibling -> K[1], sizeof(int)*(bestSibling -> n-1));
      memcpy(bestSibling -> Pt, &bestSibling -> Pt[1], sizeof(TerminalNode *)*(bestSibling -> n));
    }
    bestSibling -> Pt[bestSibling -> n] = NULL;
    bestSibling -> n--;
    x -> n++;
    free(stack);
    free(iStack);
    return;
  }

  if (b < i) {                                                                                                      /* case of internal node merge */
    bestSibling -> K[bestSibling -> n] = y -> K[i-1];
    memcpy(&bestSibling -> K[bestSibling -> n+1], x -> K, sizeof(int)*x -> n);
    memcpy(&bestSibling -> Pt[bestSibling -> n+1], x -> Pt, sizeof(TerminalNode *)*(x -> n+1));
    memcpy(&y -> K[i-1], &y -> K[i], sizeof(int)*(y -> n-i));
    memcpy(&y -> Pi[i], &y -> Pi[i+1], sizeof(InternalNode *)*(y -> n-i));
    bestSibling -> n += x -> n+1;
    free(x);
  } else {
    x -> K[x -> n] = y -> K[i];
    memcpy(&x -> K[x -> n+1], bestSibling -> K, sizeof(int)*bestSibling -> n);
    memcpy(&x -> Pt[x -> n+1], bestSibling -> Pt, sizeof(TerminalNode *)*(bestSibling -> n+1));
    memcpy(&y -> K[i], &y -> K[i+1], sizeof(int)*(y -> n-i-1));
    memcpy(&y -> Pi[i+1], &y -> Pi[i+2], sizeof(InternalNode *)*(y -> n-i-1));
    x -> n += bestSibling -> n+1;
    free(bestSibling);
  }
  y -> Pi[y -> n] = NULL;
  y -> n--;
  x = y;

  while (0 <= --size) {
    if  ((m-1)/2 <= x -> n) { free(stack); free(iStack); return; }

    y           = stack[size];
    i           = iStack[size];
    b           = i == 0 ? i+1 : i == y -> n ? i-1 : y -> Pi[i-1] -> n < y -> Pi[i+1] -> n ? i+1 : i-1;             /* choose bestSibling of x node */
    bestSibling = y -> Pi[b];

    if    ((m-1)/2 < bestSibling -> n) {                                                                            /* case of key redistribution */
      if  (b < i) {
        memcpy(&x -> K[1], x -> K, sizeof(int)*x -> n);
        memcpy(&x -> Pi[1], x -> Pi, sizeof(InternalNode *)*(x -> n+1));
        x -> K[0]   = y -> K[i-1];
        y -> K[i-1] = bestSibling -> K[bestSibling -> n-1];
        x -> Pi[0]  = bestSibling -> Pi[bestSibling -> n];
      } else {
        x -> K[x -> n]    = y -> K[i];
        y -> K[i]         = bestSibling -> K[0];
        x -> Pi[x -> n+1] = bestSibling -> Pi[0];
        memcpy(bestSibling -> K, &bestSibling -> K[1], sizeof(int)*(bestSibling -> n-1));
        memcpy(bestSibling -> Pi, &bestSibling -> Pi[1], sizeof(InternalNode *)*bestSibling -> n);
      }
      bestSibling -> Pi[bestSibling -> n] = NULL;
      bestSibling -> n--;
      x -> n++;
      break;
    }

    if (b < i) {                                                                                                    /* case of internal node merge */
      bestSibling -> K[bestSibling -> n] = y -> K[i-1];
      memcpy(&bestSibling -> K[bestSibling -> n+1], x -> K, sizeof(int)*x -> n);
      memcpy(&bestSibling -> Pi[bestSibling -> n+1], x -> Pi, sizeof(InternalNode *)*(x -> n+1));
      memcpy(&y -> K[i-1], &y -> K[i], sizeof(int)*(y -> n-i));
      memcpy(&y -> Pi[i], &y -> Pi[i+1], sizeof(InternalNode *)*(y -> n-i));
      bestSibling -> n += x -> n+1;
      free(x);
    } else {
      x -> K[x -> n] = y -> K[i];
      memcpy(&x -> K[x -> n+1], bestSibling -> K, sizeof(int)*bestSibling -> n);
      memcpy(&x -> Pi[x -> n+1], bestSibling -> Pi, sizeof(InternalNode *)*(bestSibling -> n+1));
      memcpy(&y -> K[i], &y -> K[i+1], sizeof(int)*(y -> n-i-1));
      memcpy(&y -> Pi[i+1], &y -> Pi[i+2], sizeof(InternalNode *)*(y -> n-i-1));
      x -> n += bestSibling -> n+1;
      free(bestSibling);
    }
    y -> Pi[y -> n] = NULL;
    y -> n--;
    x = y;
  }

  if (x -> n == 0) { (*T) -> IndexSet = x -> Pi[0]; free(x); }                                                      /* the level of tree decreases */

  free(stack);
  free(iStack);
}

/**
 * traverseBPT implements sequential access in T.
 * @param T: a B+-tree
 */
void traverseBPT(Tree T) { if (T != NULL) { for (TerminalNode *node = T -> SequenceSet; node != NULL; node = node -> P) { for (int i=0; i<node -> q; i++) { printf("%d ", node -> K[i]); } } } }
