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

#include <stdio.h>
#include <string.h>

/**
 * TerminalNode represents a terminal node in B+-tree.
 */
typedef struct TerminalNode {
  int                 q,  *K;
  struct TerminalNode *P;
} TerminalNode;

/**
 * InternalNode represents an internal node in B+-tree.
 */
typedef struct InternalNode {
  int                 n,  *K;
  struct InternalNode **Pi;
  TerminalNode        **Pt;
} InternalNode;

typedef struct Tree {
  InternalNode *IndexSet;
  TerminalNode *SequenceSet;
} *Tree;

/**
 * insertBPT inserts newKey into T.
 * @param T: a B+-tree
 * @param m: fanout of B+-tree
 * @param newKey: a key to insert
 */
void insertBPT(Tree *T, const int m, const int newKey);

/**
 * deleteBPT deletes oldKey from T.
 * @param T: a B+-tree
 * @param m: fanout of B+-tree
 * @param oldKey: a key to delete
 */
void deleteBPT(Tree *T, const int m, const int oldKey);

/**
 * traverseBPT implements sequential access in T.
 * @param T: a B+-tree
 */
void traverseBPT(const Tree T);

#endif
