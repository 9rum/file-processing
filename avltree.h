/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 *
 * avltree.h - generic AVL tree implementation
 *
 * The AVL tree (named after inventors Adelson-Velsky and Landis)
 * is a self-balancing binary search tree.
 *
 * In an AVL tree, the heights of the two child subtrees of any node
 * differ by at most one; if at any time they differ by more than one,
 * rebalancing is done to restore this property.
 *
 * Lookup, insertion, and deletion all take O(log n) time in both
 * the average and worst cases, where n is the number of nodes
 * in the tree prior to the operation.
 */
#ifndef _AVLTREE_H
#define _AVLTREE_H

#include <stdint.h>

#include "include/stack.h"

/**
 * struct avl_node - a node in AVL tree
 *
 * @key:    the key of the node
 * @value:  the value of the node
 * @left:   the pointer to the left subtree
 * @right:  the pointer to the right subtree
 * @height: the height of the subtree rooted with the node
 * @bf:     the balance factor of the node
 *
 * In a binary tree, the balance factor of a node X is defined
 * to be the height difference
 *
 * BF(X) := Height(LeftSubtree(X)) - Height(RightSubtree(X))
 *
 * of its two child subtrees.
 * A binary tree is defined to be an AVL tree if the invariant
 *
 * BF(X) ∈ {-1, 0, 1}
 *
 * holds for every node X in the tree.
 *
 * See https://zhjwpku.com/assets/pdf/AED2-10-avl-paper.pdf
 */
struct avl_node {
  const void            *key;
        void            *value;
        struct avl_node *left;
        struct avl_node *right;
        uint32_t        height;
        int32_t         bf;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__)));

/**
 * avl_get_node - returns a new struct avl_node
 */
static inline struct avl_node *avl_get_node() {
  struct avl_node *node = malloc(sizeof(struct avl_node));
  node->left            = NULL;
  node->right           = NULL;
  node->height          = 1;
  node->bf              = 0;
  return node;
}

static inline uint32_t max(const uint32_t a, const uint32_t b) { return a < b ? b : a; }

/**
 * height - returns the height of @tree
 *
 * @tree: tree to get the height
 */
static inline uint32_t height(const struct avl_node *tree) { return tree == NULL ? 0 : tree->height; }

/*
 * The below functions use the operator with 3 different
 * calling conventions. The operator denotes:
 *
 *    a < b  := less(a, b)
 *    a > b  := less(b, a)
 *    a == b := !less(a, b) && !less(b, a)
 *
 * NOTE:
 *
 * less must describe a transitive ordering:
 *  - if both less(a, b) and less(b, c) are true, then less(a, c) must be true as well
 *  - if both less(a, b) and less(b, c) are false, then less(a, c) must be false as well
 */

/**
 * avl_rotate_LL - implements Left Left rotation in subtree rooted with @x
 *
 * @root:   root node of tree
 * @x:      root node of subtree
 * @parent: parent node of @x
 * @less:   operator defining the (partial) node order
 */
static inline void avl_rotate_LL(struct avl_node **root, struct avl_node *x, struct avl_node *parent,
                                 bool (*less)(const void *, const void *)) {
  struct avl_node *lchild = x->left;
  x->left                 = lchild->right;
  lchild->right           = x;

  if      (parent == NULL)    *root         = lchild;  /* case of root */
  else if (parent->left == x) parent->left  = lchild;
  else                        parent->right = lchild;

  register struct avl_node *cursor = *root;
           struct stack    *stack  = NULL;

  while (cursor != x->left && cursor != x->right) {
    push(&stack, cursor);
    cursor = less(x->key, cursor->key) ? cursor->left : cursor->right;
  }

  while (!empty(stack)) {
    cursor         = pop(&stack);
    cursor->height = 1 + max(height(cursor->left), height(cursor->right));
    cursor->bf     = height(cursor->left) - height(cursor->right);
  }
}

/**
 * avl_rotate_RR - implements Right Right rotation in subtree rooted with @x
 *
 * @root:   root node of tree
 * @x:      root node of subtree
 * @parent: parent node of @x
 * @less:   operator defining the (partial) node order
 */
static inline void avl_rotate_RR(struct avl_node **root, struct avl_node *x, struct avl_node *parent,
                                 bool (*less)(const void *, const void *)) {
  struct avl_node *rchild = x->right;
  x->right                = rchild->left;
  rchild->left            = x;

  if      (parent == NULL)    *root         = rchild;  /* case of root */
  else if (parent->left == x) parent->left  = rchild;
  else                        parent->right = rchild;

  register struct avl_node *cursor = *root;
           struct stack    *stack  = NULL;

  while (cursor != x->left && cursor != x->right) {
    push(&stack, cursor);
    cursor = less(x->key, cursor->key) ? cursor->left : cursor->right;
  }

  while (!empty(stack)) {
    cursor         = pop(&stack);
    cursor->height = 1 + max(height(cursor->left), height(cursor->right));
    cursor->bf     = height(cursor->left) - height(cursor->right);
  }
}

/**
 * avl_rotate_LR - implements Left Right rotation in subtree rooted with @x
 *
 * @root:   root node of tree
 * @x:      root node of subtree
 * @parent: parent node of @x
 * @less:   operator defining the (partial) node order
 */
static inline void avl_rotate_LR(struct avl_node **root, struct avl_node *x, struct avl_node *parent,
                                 bool (*less)(const void *, const void *)) {
  struct avl_node *lchild  = x->left;
  struct avl_node *rgchild = lchild->right;
  lchild->right            = rgchild->left;
  x->left                  = rgchild->right;
  rgchild->left            = lchild;
  rgchild->right           = x;

  if      (parent == NULL)    *root         = rgchild; /* case of root */
  else if (parent->left == x) parent->left  = rgchild;
  else                        parent->right = rgchild;

  register struct avl_node *cursor = *root;
           struct stack    *stack  = NULL;

  while (cursor != x->left && cursor != x->right) {
    push(&stack, cursor);
    cursor = less(x->key, cursor->key) ? cursor->left : cursor->right;
  }
  push(&stack, lchild);

  while (!empty(stack)) {
    cursor         = pop(&stack);
    cursor->height = 1 + max(height(cursor->left), height(cursor->right));
    cursor->bf     = height(cursor->left) - height(cursor->right);
  }
}

/**
 * avl_rotate_RL - implements Right Left rotation in subtree rooted with @x
 *
 * @root:   root node of tree
 * @x:      root node of subtree
 * @parent: parent node of @x
 * @less:   operator defining the (partial) node order
 */
static inline void avl_rotate_RL(struct avl_node **root, struct avl_node *x, struct avl_node *parent,
                                 bool (*less)(const void *, const void *)) {
  struct avl_node *rchild  = x->right;
  struct avl_node *lgchild = rchild->left;
  x->right                 = lgchild->left;
  rchild->left             = lgchild->right;
  lgchild->left            = x;
  lgchild->right           = rchild;

  if      (parent == NULL)    *root         = lgchild; /* case of root */
  else if (parent->left == x) parent->left  = lgchild;
  else                        parent->right = lgchild;

  register struct avl_node *cursor = *root;
           struct stack    *stack  = NULL;

  while (cursor != x->left && cursor != x->right) {
    push(&stack, cursor);
    cursor = less(x->key, cursor->key) ? cursor->left : cursor->right;
  }
  push(&stack, rchild);

  while (!empty(stack)) {
    cursor         = pop(&stack);
    cursor->height = 1 + max(height(cursor->left), height(cursor->right));
    cursor->bf     = height(cursor->left) - height(cursor->right);
  }
}

/**
 * avl_insert - inserts @key and @value into @tree
 *
 * @tree:  tree to insert @key and @value into
 * @key:   the key to insert
 * @value: the value to insert
 * @less:  operator defining the (partial) node order
 */
extern inline void avl_insert(struct avl_node **tree, const void *key, void *value,
                              bool (*less)(const void *, const void *)) {
  register struct avl_node *cursor = *tree;
  register struct avl_node *x      = NULL;
           struct avl_node *parent = NULL;
           struct stack    *stack  = NULL;

  push(&stack, NULL);

  while (cursor != NULL) {                                      /* Phase 1: find position to insert @key and @value */
    if  (!(less(key, cursor->key) || less(cursor->key, key))) { cursor->value = value; clear(&stack); return; }
    push(&stack, cursor);
    cursor = less(key, cursor->key) ? cursor->left : cursor->right;
  }

  struct avl_node *node = avl_get_node();                       /* Phase 2: insert @key and @value and rebalance */
  node->key             = key;
  node->value           = value;

  if      ((cursor = top(stack)) == NULL) *tree         = node;
  else if (less(key, cursor->key))        cursor->left  = node;
  else                                    cursor->right = node;

  while (top(stack) != NULL && x == NULL) {
    cursor         = pop(&stack);
    cursor->height = 1 + max(height(cursor->left), height(cursor->right));
    cursor->bf     = height(cursor->left) - height(cursor->right);
    if (1 < cursor->bf || cursor->bf < -1) { x = cursor; parent = top(stack); }
  }

  clear(&stack);

  if (x == NULL) return;

  if   (1 < x->bf) {
    if (x->left->bf < 0)  avl_rotate_LR(tree, x, parent, less); /* case of Left Right */
    else                  avl_rotate_LL(tree, x, parent, less); /* case of Left Left */
  } else {
    if (0 < x->right->bf) avl_rotate_RL(tree, x, parent, less); /* case of Right Left */
    else                  avl_rotate_RR(tree, x, parent, less); /* case of Right Right */
  }
}

/**
 * avl_erase - erases @key from @tree
 *
 * @tree: tree to erase @key from
 * @key:  the key to erase
 * @less: operator defining the (partial) node order
 */
extern inline void avl_erase(struct avl_node **tree, const void *key,
                             bool (*less)(const void *, const void *)) {
  register struct avl_node *cursor = *tree;
  register struct avl_node *x      = NULL;
           struct avl_node *parent = NULL;
           struct stack    *stack  = NULL;

  push(&stack, NULL);

  while (cursor != NULL && (less(key, cursor->key) || less(cursor->key, key))) {
    push(&stack, cursor);
    cursor = less(key, cursor->key) ? cursor->left : cursor->right;
  }

  if (cursor == NULL) { clear(&stack); return; }

  if (cursor->left != NULL && cursor->right != NULL) {                /* case of degree 2 */
    parent = cursor;
    push(&stack, cursor);

    if (cursor->bf <= 0) for (cursor = cursor->right; cursor->left != NULL; cursor = cursor->left)  push(&stack, cursor);
    else                 for (cursor = cursor->left; cursor->right != NULL; cursor = cursor->right) push(&stack, cursor);

    parent->key   = cursor->key;
    parent->value = cursor->value;
  }

  parent = top(stack);

  if          (cursor->left == NULL && cursor->right == NULL) {       /* case of degree 0 */
    if        (parent == NULL)         *tree         = NULL;          /* case of root */
    else if   (parent->left == cursor) parent->left  = NULL;
    else                               parent->right = NULL;
  } else {                                                            /* case of degree 1 */
    if        (cursor->left != NULL) {
      if      (parent == NULL)         *tree         = cursor->left;  /* case of root */
      else if (parent->left == cursor) parent->left  = cursor->left;
      else                             parent->right = cursor->left;
    } else {
      if      (parent == NULL)         *tree         = cursor->right; /* case of root */
      else if (parent->left == cursor) parent->left  = cursor->right;
      else                             parent->right = cursor->right;
    }
  }

  free(cursor);

  while (top(stack) != NULL && x == NULL) {
    cursor         = pop(&stack);
    cursor->height = 1 + max(height(cursor->left), height(cursor->right));
    cursor->bf     = height(cursor->left) - height(cursor->right);
    if (1 < cursor->bf || cursor->bf < -1) { x = cursor; parent = top(stack); }
  }

  clear(&stack);

  if (x == NULL) return;

  if   (1 < x->bf) {
    if (x->left->bf < 0)  avl_rotate_LR(tree, x, parent, less);       /* case of Left Right */
    else                  avl_rotate_LL(tree, x, parent, less);       /* case of Left Left */
  } else {
    if (0 < x->right->bf) avl_rotate_RL(tree, x, parent, less);       /* case of Right Left */
    else                  avl_rotate_RR(tree, x, parent, less);       /* case of Right Right */
  }
}

/**
 * avl_preorder - implements preorder traversal in @tree
 *
 * @tree: tree to traverse
 * @f:    function to apply to each node of @tree
 */
extern inline void avl_preorder(const struct avl_node *tree, void (*f)(const struct avl_node *)) { if (tree != NULL) { f(tree); avl_preorder(tree->left, f); avl_preorder(tree->right, f); } }

/**
 * avl_inorder - implements inorder traversal in @tree
 *
 * @tree: tree to traverse
 * @f:    function to apply to each node of @tree
 */
extern inline void avl_inorder(const struct avl_node *tree, void (*f)(const struct avl_node *)) { if (tree != NULL) { avl_inorder(tree->left, f); f(tree); avl_inorder(tree->right, f); } }

/**
 * avl_postorder - implements postorder traversal in @tree
 *
 * @tree: tree to traverse
 * @f:    function to apply to each node of @tree
 */
extern inline void avl_postorder(const struct avl_node *tree, void (*f)(const struct avl_node *)) { if (tree != NULL) { avl_postorder(tree->left, f); avl_postorder(tree->right, f); f(tree); } }

#endif /* _AVLTREE_H */
