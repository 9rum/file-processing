/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * rbtree.h
 * Red-black tree implementation
 */

#ifndef _RBTREE_H
#define _RBTREE_H

#include <stdio.h>

typedef enum color_t { RED, BLACK } color_t;

/**
 * Node represents a node in Red-black tree.
 * @see https://docs.lib.purdue.edu/cgi/viewcontent.cgi?article=1457&context=cstech
 */
typedef struct Node {
  int         key;
  color_t     color;
  struct Node *left,  *right;
} Node;

typedef Node *Tree;

/**
 * insertRB inserts newKey into T.
 * @param T: a Red-black tree
 * @param newKey: a key to insert
 */
void insertRB(Tree *T, const int newKey);

/**
 * deleteRB deletes deleteKey from T.
 * @param T: a Red-black tree
 * @param deleteKey: a key to delete
 */
void deleteRB(Tree *T, const int deleteKey);

/**
 * inorderRB implements inorder traversal in T.
 * @param T: a Red-black tree
 */
void inorderRB(const Tree T);

#endif /* _RBTREE_H */
