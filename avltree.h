/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * avltree.h
 * AVL tree implementation
 */

#ifndef _AVLTREE_H
#define _AVLTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Node represents a node in AVL tree.
 * @see https://zhjwpku.com/assets/pdf/AED2-10-avl-paper.pdf
 */
typedef struct Node {
  int         key,    height, bf;
  struct Node *left,  *right;
} Node;

typedef Node *Tree;

/**
 * insertAVL inserts newKey into T.
 * @param T: an AVL tree
 * @param newKey: a key to insert
 */
void insertAVL(Tree *T, const int newKey);

/**
 * deleteAVL deletes deleteKey from T.
 * @param T: an AVL tree
 * @param deleteKey: a key to delete
 */
void deleteAVL(Tree *T, const int deleteKey);

/**
 * inorderAVL implements inorder traversal in T.
 * @param T: an AVL tree
 */
void inorderAVL(const Tree T);

#endif
