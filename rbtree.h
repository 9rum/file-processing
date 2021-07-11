/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 *
 * rbtree.h - generic red-black tree implementation
 *
 * The red-black tree is a kind of self-balancing binary search tree.
 * Each node stores an extra bit representing "color" ("red" or "black"),
 * used to ensure that the tree remains balanced during insertions and deletions.
 *
 * When the tree is modified, the new tree is rearranged and recolored
 * to restore the coloring properties that constrain how unbalanced
 * the tree can become in the worst case.
 *
 * The rebalancing is not perfect, but guarantees searching in O(log n) time,
 * where n is the number of nodes in the tree.
 * The insertion and deletion, along with the tree rearranging and recoloring,
 * are also performed in O(log n) time.
 */
#ifndef _RBTREE_H
#define _RBTREE_H

#include <stack.h>

/**
 * struct rb_node - a node in red-black tree
 *
 * @key:   the key of the node
 * @value: the value of the node
 * @left:  the pointer to the left subtree
 * @right: the pointer to the right subtree
 * @color: the color of the node
 *
 * In addition to the requirements imposed on a binary search tree,
 * the following must be satisfied by a red–black tree:
 *
 *  1. Each node is either red or black
 *  2. The root node is black
 *  3. All NIL leaves are black
 *  4. Both children of every red node are black
 *  5. Every simple path from a given node to any of its descendant NIL leaves
 *     goes through the same number of black nodes
 *
 * See https://docs.lib.purdue.edu/cgi/viewcontent.cgi?article=1457&context=cstech
 */
struct rb_node {
  const void                *key;
        void                *value;
        struct rb_node      *left;
        struct rb_node      *right;
        enum { RED, BLACK } color;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__)));

/**
 * rb_get_node - returns a new struct rb_node
 */
static inline struct rb_node *rb_get_node(void) {
  struct rb_node *node = malloc(sizeof(struct rb_node));
  node->left           = NULL;
  node->right          = NULL;
  node->color          = RED;
  return node;
}

/**
 * rb_rotate_left - rotates subtree rooted with @node counterclockwise
 *
 * @root:   root node of tree
 * @node:   root node of subtree
 * @parent: parent node of @node
 */
static inline void rb_rotate_left(struct rb_node **restrict root, struct rb_node *restrict node, struct rb_node *restrict parent) {
  struct rb_node *rchild = node->right;
  node->right            = rchild->left;
  rchild->left           = node;

  if      (parent == NULL)       *root         = rchild; /* case of root */
  else if (parent->left == node) parent->left  = rchild;
  else                           parent->right = rchild;
}

/**
 * rb_rotate_right - rotates subtree rooted with @node clockwise
 *
 * @root:   root node of tree
 * @node:   root node of subtree
 * @parent: parent node of @node
 */
static inline void rb_rotate_right(struct rb_node **restrict root, struct rb_node *restrict node, struct rb_node *restrict parent) {
  struct rb_node *lchild = node->left;
  node->left             = lchild->right;
  lchild->right          = node;

  if      (parent == NULL)       *root         = lchild; /* case of root */
  else if (parent->left == node) parent->left  = lchild;
  else                           parent->right = lchild;
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
 * rb_insert - inserts @key and @value into @tree
 *
 * @tree:  tree to insert @key and @value into
 * @key:   the key to insert
 * @value: the value to insert
 * @less:  operator defining the (partial) node order
 */
extern inline void rb_insert(struct rb_node **restrict tree, const void *restrict key, void *restrict value, bool (*less)(const void *, const void *)) {
  register struct rb_node *walk  = *tree;
  register struct rb_node *parent;
  register struct rb_node *gparent;
  register struct rb_node *uncle;
           struct stack   *stack = NULL;

  while (walk != NULL) {
    if  (!(less(key, walk->key) || less(walk->key, key))) { destroy(&stack); return; }
    push(&stack, walk);
    walk = less(key, walk->key) ? walk->left : walk->right;
  }

  walk        = rb_get_node();
  walk->key   = key;
  walk->value = value;

  if      ((parent = top(stack)) == NULL) *tree         = walk, walk->color = BLACK;
  else if (less(key, parent->key))        parent->left  = walk;
  else                                    parent->right = walk;

  while (!empty(stack)) {
    if  ((parent = pop(&stack))->color == BLACK) { destroy(&stack); return; }

    gparent = pop(&stack);
    uncle   = gparent->right == parent ? gparent->left : gparent->right;

    if     (uncle == NULL || uncle->color == BLACK) { /* case of rearranging */
      if   (gparent->left == parent) {
        if (parent->left == walk) {                   /* case of Left Left */
          parent->color  = BLACK;
          gparent->color = RED;
          rb_rotate_right(tree, gparent, top(stack));
        } else {                                      /* case of Left Right */
          walk->color    = BLACK;
          gparent->color = RED;
          rb_rotate_left(tree, parent, gparent);
          rb_rotate_right(tree, gparent, top(stack));
        }
      } else {
        if (parent->left == walk) {                   /* case of Right Left */
          walk->color    = BLACK;
          gparent->color = RED;
          rb_rotate_right(tree, parent, gparent);
          rb_rotate_left(tree, gparent, top(stack));
        } else {                                      /* case of Right Right */
          parent->color  = BLACK;
          gparent->color = RED;
          rb_rotate_left(tree, gparent, top(stack));
        }
      }

      destroy(&stack);
      return;
    }

    parent->color = BLACK;                            /* case of recoloring */
    uncle->color  = BLACK;
    walk          = gparent;
    walk->color   = empty(stack) ? BLACK : RED;
  }
}

/**
 * rb_erase - erases @key from @tree
 *
 * @tree: tree to erase @key from
 * @key:  the key to erase
 * @less: operator defining the (partial) node order
 */
extern inline void rb_erase(struct rb_node **restrict tree, const void *restrict key, bool (*less)(const void *, const void *)) {
  register struct rb_node *walk  = *tree;
  register struct rb_node *parent;
  register struct rb_node *sibling;
           struct stack   *stack = NULL;

  while (walk != NULL && (less(key, walk->key) || less(walk->key, key))) {
    push(&stack, walk);
    walk = less(key, walk->key) ? walk->left : walk->right;
  }

  if (walk == NULL) { destroy(&stack); return; }

  if (walk->left != NULL && walk->right != NULL) {                    /* case of degree 2 */
    parent = walk;
    push(&stack, walk);

    for (walk = walk->left; walk->right != NULL; walk = walk->right) push(&stack, walk);

    parent->key   = walk->key;
    parent->value = walk->value;
  }

  if          (walk->left == NULL && walk->right == NULL) {           /* case of degree 0 */
    if        ((parent = top(stack)) == NULL) *tree         = NULL;
    else if   (parent->left == walk)          parent->left  = NULL;
    else                                      parent->right = NULL;
  } else {                                                            /* case of degree 1 */
    if        (walk->left != NULL) {
      if      ((parent = top(stack)) == NULL) *tree         = walk->left;
      else if (parent->left == walk)          parent->left  = walk->left;
      else                                    parent->right = walk->left;
    } else {
      if      ((parent = top(stack)) == NULL) *tree         = walk->right;
      else if (parent->left == walk)          parent->left  = walk->right;
      else                                    parent->right = walk->right;
    }
  }

  if (walk->color == RED) { free(walk); destroy(&stack); return; }

  parent = walk;
  walk   = parent->right == NULL ? parent->left : parent->right;
  free(parent);

  if (walk != NULL && walk->color == RED) { walk->color = BLACK; destroy(&stack); return; }

  while (!empty(stack)) {
    parent  = pop(&stack);
    sibling = parent->right == walk ? parent->left : parent->right;

    if (sibling->color == RED) {                                      /* case of rearranging */
      sibling->color = BLACK;
      parent->color  = RED;
      parent->left == walk ? rb_rotate_left(tree, parent, top(stack)) : rb_rotate_right(tree, parent, top(stack));
      push(&stack, sibling);
      sibling        = parent->right == walk ? parent->left : parent->right;
    }

    if     (sibling->left != NULL && sibling->left->color == RED ||
            sibling->right != NULL && sibling->right->color == RED) { /* case of rearranging */
      if   (parent->left == sibling) {
        if (sibling->right != NULL && sibling->right->color == RED) { /* case of Left Right */
          sibling->right->color = BLACK;
          sibling->color        = RED;
          rb_rotate_left(tree, sibling, parent);
          sibling               = parent->left;
        }
        sibling->left->color = BLACK;                                 /* case of Left Left */
        sibling->color       = parent->color;
        parent->color        = BLACK;
        rb_rotate_right(tree, parent, top(stack));
      } else {
        if (sibling->left != NULL && sibling->left->color == RED) {   /* case of Right Left */
          sibling->left->color = BLACK;
          sibling->color       = RED;
          rb_rotate_right(tree, sibling, parent);
          sibling              = parent->right;
        }
        sibling->right->color = BLACK;                                /* csae of Right Right */
        sibling->color        = parent->color;
        parent->color         = BLACK;
        rb_rotate_left(tree, parent, top(stack));
      }

      destroy(&stack);
      return;
    }

    sibling->color = RED;                                             /* case of recoloring */
    if (parent->color == RED) { parent->color = BLACK; destroy(&stack); return; }
    walk           = parent;
  }
}

/**
 * rb_preorder - applies @func to each node of @tree preorderwise
 *
 * @tree: tree to apply @func to each node of
 * @func: function to apply to each node of @tree
 */
extern inline void rb_preorder(const struct rb_node *restrict tree, void (*func)(const struct rb_node *restrict)) { if (tree != NULL) { func(tree); rb_preorder(tree->left, func); rb_preorder(tree->right, func); } }

/**
 * rb_inorder - applies @func to each node of @tree inorderwise
 *
 * @tree: tree to apply @func to each node of
 * @func: function to apply to each node of @tree
 */
extern inline void rb_inorder(const struct rb_node *restrict tree, void (*func)(const struct rb_node *restrict)) { if (tree != NULL) { rb_inorder(tree->left, func); func(tree); rb_inorder(tree->right, func); } }

/**
 * rb_postorder - applies @func to each node of @tree postorderwise
 *
 * @tree: tree to apply @func to each node of
 * @func: function to apply to each node of @tree
 */
extern inline void rb_postorder(const struct rb_node *restrict tree, void (*func)(const struct rb_node *restrict)) { if (tree != NULL) { rb_postorder(tree->left, func); rb_postorder(tree->right, func); func(tree); } }

#endif /* _RBTREE_H */
