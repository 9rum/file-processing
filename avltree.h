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

/**
 * avl_rotate_left - implements Left rotation in subtree rooted with @x
 *
 * @root:   root node of tree
 * @x:      root node of subtree
 * @parent: parent node of @x
 */
static inline void avl_rotate_left(struct avl_node **root, struct avl_node *x, struct avl_node *parent) {
  struct avl_node *rchild = x->right;
  x->right                = rchild->left;
  rchild->left            = x;

  if      (parent == NULL)    *root         = rchild; /* case of root */
  else if (parent->left == x) parent->left  = rchild;
  else                        parent->right = rchild;
}

/**
 * avl_rotate_right - implements Right rotation in subtree rooted with @x
 *
 * @root:   root node of tree
 * @x:      root node of subtree
 * @parent: parent node of @x
 */
static inline void avl_rotate_right(struct avl_node **root, struct avl_node *x, struct avl_node *parent) {
  struct avl_node *lchild = x->left;
  x->left                 = lchild->right;
  lchild->right           = x;

  if      (parent == NULL)    *root         = lchild; /* case of root */
  else if (parent->left == x) parent->left  = lchild;
  else                        parent->right = lchild;
}

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
 * avl_insert - inserts @key and @value into @tree
 *
 * @tree:  tree to insert @key and @value into
 * @key:   the key to insert
 * @value: the value to insert
 * @less:  operator defining the (partial) node order
 */
extern inline void avl_insert(struct avl_node **tree, const void *key, void *value,
                              bool (*less)(const void *, const void *)) {
  register struct avl_node *walk   = *tree;
           struct avl_node *x      = NULL;
           struct avl_node *parent = NULL;
           struct stack    *stack  = NULL;

  while (walk != NULL) {
    if  (!(less(key, walk->key) || less(walk->key, key))) { destroy(&stack); return; }
    push(&stack, walk);
    walk = less(key, walk->key) ? walk->left : walk->right;
  }

  struct avl_node *node = avl_get_node();
  node->key             = key;
  node->value           = value;

  if      ((walk = top(stack)) == NULL) *tree       = node;
  else if (less(key, walk->key))        walk->left  = node;
  else                                  walk->right = node;

  while (!empty(stack)) {
    walk         = pop(&stack);
    walk->height = 1 + max(height(walk->left), height(walk->right));
    walk->bf     = height(walk->left) - height(walk->right);
    if (1 < walk->bf || walk->bf < -1) { x = walk; parent = top(stack); break; }
  }

  if   (x == NULL) return;

  if   (1 < x->bf) {
    if (x->left->bf < 0) {  /* case of Left Right */
      push(&stack, x->left->right);
      push(&stack, x->left);
      push(&stack, x);
      avl_rotate_left(tree, x->left, x);
      avl_rotate_right(tree, x, parent);
    } else {                /* case of Left Left */
      push(&stack, x->left);
      push(&stack, x);
      avl_rotate_right(tree, x, parent);
    }
  } else {
    if (0 < x->right->bf) { /* case of Right Left */
      push(&stack, x->right->left);
      push(&stack, x->right);
      push(&stack, x);
      avl_rotate_right(tree, x->right, x);
      avl_rotate_left(tree, x, parent);
    } else {                /* case of Right Right */
      push(&stack, x->right);
      push(&stack, x);
      avl_rotate_left(tree, x, parent);
    }
  }

  while (!empty(stack)) {
    walk         = pop(&stack);
    walk->height = 1 + max(height(walk->left), height(walk->right));
    walk->bf     = height(walk->left) - height(walk->right);
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
  register struct avl_node *walk   = *tree;
           struct avl_node *x      = NULL;
           struct avl_node *parent = NULL;
           struct stack    *stack  = NULL;

  while (walk != NULL && (less(key, walk->key) || less(walk->key, key))) {
    push(&stack, walk);
    walk = less(key, walk->key) ? walk->left : walk->right;
  }

  if (walk == NULL) { destroy(&stack); return; }

  if (walk->left != NULL && walk->right != NULL) {                         /* case of degree 2 */
    parent = walk;
    push(&stack, walk);

    if (walk->bf <= 0) for (walk = walk->right; walk->left != NULL; walk = walk->left)  push(&stack, walk);
    else               for (walk = walk->left; walk->right != NULL; walk = walk->right) push(&stack, walk);

    parent->key   = walk->key;
    parent->value = walk->value;
  }

  if          (walk->left == NULL && walk->right == NULL) {                /* case of degree 0 */
    if        ((parent = top(stack)) == NULL) *tree         = NULL;        /* case of root */
    else if   (parent->left == walk)          parent->left  = NULL;
    else                                      parent->right = NULL;
  } else {                                                                 /* case of degree 1 */
    if        (walk->left != NULL) {
      if      ((parent = top(stack)) == NULL) *tree         = walk->left;  /* case of root */
      else if (parent->left == walk)          parent->left  = walk->left;
      else                                    parent->right = walk->left;
    } else {
      if      ((parent = top(stack)) == NULL) *tree         = walk->right; /* case of root */
      else if (parent->left == walk)          parent->left  = walk->right;
      else                                    parent->right = walk->right;
    }
  }

  free(walk);

  while (!empty(stack)) {
    walk         = pop(&stack);
    walk->height = 1 + max(height(walk->left), height(walk->right));
    walk->bf     = height(walk->left) - height(walk->right);
    if (1 < walk->bf || walk->bf < -1) { x = walk; parent = top(stack); break; }
  }

  if   (x == NULL) return;

  if   (1 < x->bf) {
    if (x->left->bf < 0) {                                                 /* case of Left Right */
      push(&stack, x->left->right);
      push(&stack, x->left);
      push(&stack, x);
      avl_rotate_left(tree, x->left, x);
      avl_rotate_right(tree, x, parent);
    } else {                                                               /* case of Left Left */
      push(&stack, x->left);
      push(&stack, x);
      avl_rotate_right(tree, x, parent);
    }
  } else {
    if (0 < x->right->bf) {                                                /* case of Right Left */
      push(&stack, x->right->left);
      push(&stack, x->right);
      push(&stack, x);
      avl_rotate_right(tree, x->right, x);
      avl_rotate_left(tree, x, parent);
    } else {                                                               /* case of Right Right */
      push(&stack, x->right);
      push(&stack, x);
      avl_rotate_left(tree, x, parent);
    }
  }

  while (!empty(stack)) {
    walk         = pop(&stack);
    walk->height = 1 + max(height(walk->left), height(walk->right));
    walk->bf     = height(walk->left) - height(walk->right);
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
