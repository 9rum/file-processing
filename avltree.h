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

/**
 * Node represents a node in AVL tree.
 * @see https://zhjwpku.com/assets/pdf/AED2-10-avl-paper.pdf
 */
typedef struct Node {
  int         key,    height, bf;
  struct Node *left,  *right;
} Node;

/**
 * getNode returns a new node.
 */
Node *getNode();

typedef Node *Tree;

/**
 * height returns the height of T.
 * @param T: an AVL tree
 */
int height(Tree T);

/**
 * rotateLL implements LL rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
void rotateLL(Tree *T, Node *x, Node *f);

/**
 * rotateRR implements RR rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
void rotateRR(Tree *T, Node *x, Node *f);

/**
 * rotateLR implements LR rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
void rotateLR(Tree *T, Node *x, Node *f);

/**
 * rotateRL implements RL rotation in subtree rooted with x.
 * @param T: an AVL tree
 * @param x: root node of subtree
 * @param f: parent node of x
 */
void rotateRL(Tree *T, Node *x, Node *f);

/**
 * insertAVL inserts newKey into T.
 * @param T: an AVL tree
 * @param newKey: a key to insert
 */
void insertAVL(Tree *T, int newKey);

/**
 * deleteAVL deletes deleteKey from T.
 * @param T: an AVL tree
 * @param deleteKey: a key to delete
 */
void deleteAVL(Tree *T, int deleteKey);

/**
 * inorderAVL implements inorder traversal in T.
 * @param T: an AVL tree
 */
void inorderAVL(Tree T);

#endif
