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

void print(const struct avl_node *node) { printf("%" PRIuPTR " ", (uintptr_t)node->key); }

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
  /*
   * 40
   * 11 40
   * 11 40 77
   * 11 33 40 77
   * 11 20 33 40 77
   * 11 20 33 40 77 90
   * 11 20 33 40 77 90 99
   * 11 20 33 40 70 77 90 99
   * 11 20 33 40 70 77 88 90 99
   * 11 20 33 40 70 77 80 88 90 99
   * 11 20 33 40 66 70 77 80 88 90 99
   * 10 11 20 33 40 66 70 77 80 88 90 99
   * 10 11 20 22 33 40 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 55 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 50 55 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 50 55 60 66 70 77 80 88 90 99
   * 10 11 20 22 25 30 33 40 44 50 55 60 66 70 77 80 88 90 99
   * 10 11 20 22 25 30 33 40 44 49 50 55 60 66 70 77 80 88 90 99
   * 10 11 20 22 25 30 33 44 49 50 55 60 66 70 77 80 88 90 99
   * 10 20 22 25 30 33 44 49 50 55 60 66 70 77 80 88 90 99
   * 10 20 22 25 30 33 44 49 50 55 60 66 70 80 88 90 99
   * 10 20 22 25 30 44 49 50 55 60 66 70 80 88 90 99
   * 10 22 25 30 44 49 50 55 60 66 70 80 88 90 99
   * 10 22 25 30 44 49 50 55 60 66 70 80 88 99
   * 10 22 25 30 44 49 50 55 60 66 70 80 88
   * 10 22 25 30 44 49 50 55 60 66 80 88
   * 10 22 25 30 44 49 50 55 60 66 80
   * 10 22 25 30 44 49 50 55 60 66
   * 10 22 25 30 44 49 50 55 60
   * 22 25 30 44 49 50 55 60
   * 25 30 44 49 50 55 60
   * 25 44 49 50 55 60
   * 25 49 50 55 60
   * 25 49 50 60
   * 25 49 60
   * 25 49
   * 49
   *
   * 40
   * 11 40
   * 11 40 77
   * 11 33 40 77
   * 11 20 33 40 77
   * 11 20 33 40 77 90
   * 11 20 33 40 77 90 99
   * 11 20 33 40 70 77 90 99
   * 11 20 33 40 70 77 88 90 99
   * 11 20 33 40 70 77 80 88 90 99
   * 11 20 33 40 66 70 77 80 88 90 99
   * 10 11 20 33 40 66 70 77 80 88 90 99
   * 10 11 20 22 33 40 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 55 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 50 55 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 50 55 60 66 70 77 80 88 90 99
   * 10 11 20 22 25 30 33 40 44 50 55 60 66 70 77 80 88 90 99
   * 10 11 20 22 25 30 33 40 44 49 50 55 60 66 70 77 80 88 90 99
   * 10 11 20 22 25 30 33 40 44 50 55 60 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 50 55 60 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 50 55 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 55 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 44 66 70 77 80 88 90 99
   * 10 11 20 22 30 33 40 66 70 77 80 88 90 99
   * 10 11 20 22 33 40 66 70 77 80 88 90 99
   * 10 11 20 33 40 66 70 77 80 88 90 99
   * 11 20 33 40 66 70 77 80 88 90 99
   * 11 20 33 40 70 77 80 88 90 99
   * 11 20 33 40 70 77 88 90 99
   * 11 20 33 40 70 77 90 99
   * 11 20 33 40 77 90 99
   * 11 20 33 40 77 90
   * 11 20 33 40 77
   * 11 33 40 77
   * 11 40 77
   * 11 40
   * 40
   *
   */
}
