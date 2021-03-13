/*
 * Copyright 2019 Jose Augusto dos Santos Goulart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"

#define LIST_SIZE 32 /* size of the lists */

void print_list(node_t* head)
{
  while (head != NULL) {
    if (head->data.address != NULL)
      printf("%u\n", *head->data.address);

    head = head->next;
  }
}

void generate_node_number(node_t* node)
{
  if (node != NULL) {
    if (data_alloc(&node->data, 1)) {
      ubyte_t value = rand();

      memcpy(node->data.address, &value, node->data.size);
    }
  }
}

int main()
{
  srand((unsigned int)time(NULL));

  node_t* head_a = NULL;
  node_t* head_b = NULL;

  if (node_alloc(&head_a) && node_alloc(&head_b)) {
    /* initialize heads */
    node_t* curr_a = head_a;
    node_t* curr_b = head_b;

    generate_node_number(curr_a);
    generate_node_number(curr_b);

    /* create rest of the lists */
    for (int i = 1; i < LIST_SIZE; i++) {
      if (node_alloc(&curr_a->next) && node_alloc(&curr_b->next)) {
        curr_a->next->prev = curr_a;
        curr_b->next->prev = curr_b;

        generate_node_number(curr_a->next);
        generate_node_number(curr_b->next);

        curr_a = curr_a->next;
        curr_b = curr_b->next;
      }
    }

    node_t* lists_union = NULL;
    list_union(head_a, head_b, &lists_union);

    node_t* lists_intersection = NULL;
    list_intersection(head_a, head_b, &lists_intersection);

    node_t* lists_difference = NULL;
    list_difference(head_a, head_b, &lists_difference);

    printf("\nUnion:\n");
    print_list(lists_union);

    printf("\nIntersection:\n");
    print_list(lists_intersection);

    printf("\nDifference:\n");
    print_list(lists_difference);

    printf("\nIs %u part of A:\n", *head_b->data.address);
    printf("%s\n", ((list_find_data(head_a, &head_b->data)) ? "true" : "false"));

    printf("\nIs B a subset of A:\n");
    printf("%s\n", (list_subset(head_a, head_b) ? "true" : "false"));

    list_free(head_a);
    list_free(head_b);
    list_free(lists_union);
    list_free(lists_intersection);
    list_free(lists_difference);
  }

  exit(EXIT_SUCCESS);
}
