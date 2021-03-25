/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * bstree.h
 * Binary search tree implementation
 */

#ifndef _BSTREE_H
#define _BSTREE_H

#include <stdio.h>

/**
 * Node represents a node in Binary search tree.
 * @see https://www.sciencedirect.com/science/article/pii/S0019995860909013
 */
typedef struct Node {
  int           key;
  unsigned int  height;
  struct Node   *left,  *right;
} Node;

typedef Node *Tree;

/**
 * insertBST inserts newKey into T.
 * @param T: a binary search tree
 * @param newKey: a key to insert
 */
void insertBST(Tree *T, const int newKey);

/**
 * deleteBST deletes deleteKey from T.
 * @param T: a binary search tree
 * @param deleteKey: a key to delete
 */
void deleteBST(Tree *T, const int deleteKey);

/**
 * inorderBST implements inorder traversal in T.
 * @param T: a binary search tree
 */
void inorderBST(const Tree T);

#endif
