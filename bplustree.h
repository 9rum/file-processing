/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 * bplustree.h
 * B+-tree implementation
 */

#ifndef _BPLUSTREE_H
#define _BPLUSTREE_H

/**
 * TerminalNode represents a terminal node in B+-tree.
 */
typedef struct TerminalNode {
  int                 q,  *K;
  struct TerminalNode *P;
} TerminalNode;

/**
 * getTerminalNode returns a new terminal node.
 * @param m: fanout of B+-tree
 */
TerminalNode *getTerminalNode(int m);

/**
 * InternalNode represents an internal node in B+-tree.
 */
typedef struct InternalNode {
  int                 n,  *K;
  struct InternalNode **Pi;
  TerminalNode        **Pt;
} InternalNode;

/**
 * getInternalNode returns a new internal node.
 * @param m: fanout of B+-tree
 */
InternalNode *getInternalNode(int m);

typedef struct Tree {
  InternalNode *IndexSet;
  TerminalNode *SequenceSet;
} *Tree;

/**
 * binarySearch returns index i where K[i-1] < key <= K[i].
 * @param K: an array
 * @param n: size of array
 * @param key: a key to search
 */
int binarySearch(int K[], int n, int key);

/**
 * insertBPT inserts newKey into T.
 * @param T: a B+-tree
 * @param m: fanout of B+-tree
 * @param newKey: a key to insert
 */
void insertBPT(Tree *T, int m, int newKey);

/**
 * deleteBPT deletes oldKey from T.
 * @param T: a B+-tree
 * @param m: fanout of B+-tree
 * @param oldKey: a key to delete
 */
void deleteBPT(Tree *T, int m, int oldKey);

/**
 * traverseBPT implements sequential access in T.
 * @param T: a B+-tree
 */
void traverseBPT(Tree T);

#endif
