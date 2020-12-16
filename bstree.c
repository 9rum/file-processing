/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * bstree.c
 * Binary search tree implementation
 */

#include <stdio.h>
#include <stdlib.h>

#define max(a, b) (a < b ? b : a)

/**
 * Node represents a single node in Binary search tree.
 * @see https://www.sciencedirect.com/science/article/pii/S0019995860909013
 */
typedef struct Node {
  int         key,    height;
  struct Node *left,  *right;
} Node;

/**
 * getNode returns a new node.
 */
Node *getNode() {
  Node *node      = malloc(sizeof(Node));
  node -> height  = 1;
  node -> left    = node -> right = NULL;
  return node;
}

typedef Node *Tree;

/**
 * height returns the height of T.
 * @param T: a binary search tree
 */
int height(Tree T) { return T == NULL ? 0 : T -> height; }

/**
 * minNode returns the node with minimum key in T.
 * @param T: a binary search tree
 */
Node *minNode(Tree T) { Node *p = T; while (p -> left != NULL) p = p -> left; return p; }

/**
 * maxNode returns the node with maximum key in T.
 * @param T: a binary search tree
 */
Node *maxNode(Tree T) { Node *p = T; while (p -> right != NULL) p = p -> right; return p; }

/**
 * insertBST inserts newKey into T.
 * @param T: a binary search tree
 * @param newKey: a key to insert
 */
void insertBST(Tree *T, int newKey) {
  Node **stack  = malloc(sizeof(Node *)*height(*T)),
       *q       = NULL,
       *p       = *T;
  int size      = 0;

  while (p != NULL) {                                 /* step 1: find position q to insert newKey */
    if (newKey == p -> key) { free(stack); return; }
    q             = p;
    stack[size++] = q;
    p             = newKey < p -> key ? p -> left : p -> right;
  }

  Node *newNode   = getNode();                        /* step 2: make a node with newKey */
  newNode -> key  = newKey;

  if      (*T == NULL)        *T          = newNode;  /* step 3: insert newKey as a child of q */
  else if (newKey < q -> key) q -> left   = newNode;
  else                        q -> right  = newNode;

  while (0 <= --size) stack[size] -> height = 1 + max(height(stack[size] -> left), height(stack[size] -> right));
  
  free(stack);
}

/**
 * deleteBST deletes deleteKey from T.
 * @param T: a binary search tree
 * @param deleteKey: a key to delete
 */
void deleteBST(Tree *T, int deleteKey) {
  Node **stack  = malloc(sizeof(Node *)*height(*T)),
       *q       = NULL,
       *p       = *T;
  int size      = 0;

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

    free(p);
  } else if (p -> left == NULL || p -> right == NULL) {       /* case of degree 1 */
    if (p -> left != NULL) {
      if      (q == NULL)       *T          = (*T) -> left;   /* case of root */
      else if (q -> left == p)  q -> left   = p -> left;
      else                      q -> right  = p -> left;
    } else {
      if      (q == NULL)       *T          = (*T) -> right;  /* case of root */
      else if (q -> left == p)  q -> left   = p -> right;
      else                      q -> right  = p -> right;
    }

    free(p);
  } else {                                                    /* case of degree 2 */
    stack[size++] = p;
    
    if (p -> left -> height <= p -> right -> height)  { p -> key = minNode(p -> right) -> key;  deleteBST(&p -> right, p -> key); }
    else                                              { p -> key = maxNode(p -> left) -> key;   deleteBST(&p -> left, p -> key);  }
  }

  while (0 <= --size) stack[size] -> height = 1 + max(height(stack[size] -> left), height(stack[size] -> right));

  free(stack);
}

/**
 * inorderBST implements inorder traversal in T.
 * @param T: a binary search tree
 */
void inorderBST(Tree T) { if (T != NULL) { inorderBST(T -> left); printf("%d ", T -> key); inorderBST(T -> right); } }

int main() {
  int testCases[] = {25, 500, 33, 49, 17, 403, 29, 105, 39, 66, 305, 44, 19, 441, 390, 12, 81, 50, 100, 999};
  
  Tree T = NULL;

  for (int i=0; i<20; i++)  { insertBST(&T, testCases[i]); inorderBST(T); printf("\n"); }
  for (int i=0; i<20; i++)  { deleteBST(&T, testCases[i]); inorderBST(T); printf("\n"); }
  for (int i=0; i<20; i++)  { insertBST(&T, testCases[i]); inorderBST(T); printf("\n"); }
  for (int i=19; 0<=i; i--) { deleteBST(&T, testCases[i]); inorderBST(T); printf("\n"); }
}