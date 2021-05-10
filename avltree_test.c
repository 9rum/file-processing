/*
 * Copyright (c) 2020, 9rum. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 * File Processing, 2020
 *
 * avltree_test.c - generic AVL tree unit test
 */
#include <stdio.h>
#include <inttypes.h>

#include "avltree.h"

bool less(const void *a, const void *b) { return (uintptr_t)a < (uintptr_t)b; }

void print(const struct avl_node *tree) { printf("%" PRIuPTR " ", (uintptr_t)tree->key); }

int main() {
  const uintptr_t testcases[] = {40, 11, 77, 33, 20, 90, 99, 70, 88, 80, 66, 10, 22, 30, 44, 55, 50, 60, 25, 49};

  struct avl_node *tree = NULL;

  for (const uintptr_t *cursor = testcases; cursor < testcases + sizeof(testcases)/sizeof(uintptr_t); ++cursor) {
    avl_insert(&tree, (void *)*cursor, NULL, less);
    avl_inorder(tree, print);
    printf("\n");
  }
  for (const uintptr_t *cursor = testcases; cursor < testcases + sizeof(testcases)/sizeof(uintptr_t); ++cursor) {
    avl_erase(&tree, (void *)*cursor, less);
    avl_inorder(tree, print);
    printf("\n");
  }
  for (const uintptr_t *cursor = testcases; cursor < testcases + sizeof(testcases)/sizeof(uintptr_t); ++cursor) {
    avl_insert(&tree, (void *)*cursor, NULL, less);
    avl_inorder(tree, print);
    printf("\n");
  }
  for (const uintptr_t *cursor = testcases + sizeof(testcases)/sizeof(uintptr_t) - 1; testcases <= cursor; --cursor) {
    avl_erase(&tree, (void *)*cursor, less);
    avl_inorder(tree, print);
    printf("\n");
  }
}
