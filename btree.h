/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * btree.h
 * B-tree implementation
 */

#ifndef _BTREE_H
#define _BTREE_H

/**
 * Node represents a node in B-tree.
 * @see https://infolab.usc.edu/csci585/Spring2010/den_ar/indexing.pdf
 */
typedef struct Node {
  int         n,  *K;
  struct Node **P;
} Node;

/**
 * getNode returns a new node.
 * @param m: fanout of B-tree
 */
Node *getNode(int m);

typedef Node *Tree;

/**
 * binarySearch returns index i where K[i-1] < key <= K[i].
 * @param K: an array
 * @param n: size of array
 * @param key: a key to search
 */
int binarySearch(int K[], int n, int key);

/**
 * insertBT inserts newKey into T.
 * @param T: a B-tree
 * @param m: fanout of B-tree
 * @param newKey: a key to insert
 */
void insertBT(Tree *T, int m, int newKey);

/**
 * deleteBT deletes oldKey from T.
 * @param T: a B-tree
 * @param m: fanout of B-tree
 * @param oldKey: a key to delete
 */
void deleteBT(Tree *T, int m, int oldKey);

/**
 * inorderBT implements inorder traversal in T.
 * @param T: a B-tree
 */
void inorderBT(Tree T);

#endif
