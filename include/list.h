/*
 * Copyright 2019-2021 Jose Augusto dos Santos Goulart
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

#ifndef _list_h_
#define _list_h_

#include <stdlib.h>
#include "memory.h"

/**
 * Double linked list node
 */
typedef struct node_s {
  /** data stored by the node */
  data_t         data;
  /** previous node on the list */
  struct node_s* prev;
  /** next node on the list */
  struct node_s* next;
} node_t;

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************
 * Free node allocated memory.
 *
 * @param node reference to node
 **********************************************************/
void node_free(node_t* node)
{
  if (node != NULL) {
    data_free(&node->data);
    free(node);
  }
}

/**********************************************************
 * Allocate memory for the node.
 *
 * @param node reference to pointer that will receive the
 *             node address
 *
 * @return bool whether the allocation was successful
 **********************************************************/
bool node_alloc(node_t** node)
{
  if (node != NULL) {
    if ((*node = malloc(sizeof(node_t))) != NULL) {
      (*node)->data.size = 0;
      (*node)->data.address = NULL;
      (*node)->prev = NULL;
      (*node)->next = NULL;

      return true;
    }
    LOG("Warning: Could not allocate memory for node.");
  }

  return false;
}

/**********************************************************
 * Copy one node to another.
 *
 * @param source node to be copied
 * @param destination where the node copy will be placed
 *
 * @return bool whether the copy was successful
 **********************************************************/
bool node_copy(node_t* source, node_t** destination)
{
  if (source != NULL && destination != NULL) {
    if (node_alloc(destination)) {
      (*destination)->prev = source->prev;
      (*destination)->next = source->next;

      return data_copy(&source->data, &(*destination)->data);
    }
  }

  LOG("Warning: Could not copy source into node.");
  return false;
}

/**********************************************************
 * Get tail from list.
 *
 * @param head fisrt node on the list
 *
 * @return node_t* pointer to tail node
 **********************************************************/
node_t* list_get_tail(node_t* head)
{
  node_t* tail = NULL;

  if (head != NULL) {
    node_t* current = head;

    while (current->next != NULL)
      current = current->next;

    tail = current;
  }

  return tail;
}

/**********************************************************
 * Append node to list.
 *
 * @param head first node on list
 * @param node node to be appended
 **********************************************************/
void list_append(node_t* head, node_t* node)
{
  if (head != NULL && node != NULL) {
    node_t* tail = list_get_tail(head);

    if (tail != NULL) {
      tail->next = node;
      node->prev = tail;
    }
  }
}

/**********************************************************
 * Find if given data is on this list.
 *
 * @param head first node on list
 * @param data data to be searched for
 *
 * @return node_t* node reference where the data was found or
 *                 NULL if not found
 **********************************************************/
node_t* list_find_data(node_t* head, const data_t* data)
{
  if (head != NULL && data != NULL) {
    node_t* current = head;

    /* iterate through list to find data */
    do {
      if (current->data.size == data->size) {
        if (!memcmp(current->data.address, data->address, data->size))
          return current;
      }

      current = current->next;
    } while (current != NULL);
  }

  return NULL;
}

/**********************************************************
 * Create new tail on destination from source node
 *
 * @param src_node node to be copied
 * @param dest_tail destination list tail
 *
 * @return bool whether operation was successful
 **********************************************************/
bool list_push(node_t* src_node, node_t* dest_tail) {
  if (node_alloc(&dest_tail->next)) {
    data_copy(&src_node->data, &dest_tail->next->data);
    dest_tail->next->prev = dest_tail;

    return true;
  }

  return false;
}

/**********************************************************
 * Make a copy of each element of source list into destination
 * list without repeating the elements of comparison list.
 *
 * @param src_head first node on source list
 * @param cmp_head comparison list head
 * @param dest_head destination list head
 * @param dest_tail destination list tail
 *
 * @return node_t* new destination list tail
 **********************************************************/
node_t* list_single_copy(node_t* src_head, node_t* cmp_head, node_t* dest_head, node_t* dest_tail)
{
  if (dest_tail != NULL && cmp_head != NULL) {
    while (src_head != NULL) {
      if (list_find_data(cmp_head, &src_head->data) == NULL &&
          list_find_data(dest_head, &src_head->data) == NULL) {
        if (list_push(src_head, dest_tail))
          dest_tail = dest_tail->next;
      }

      src_head = src_head->next;
    }
  }

  return dest_tail;
}

/**********************************************************
 * Make a copy of each element of source list into destination
 * list, but only if the element is also on comparison list.
 *
 * @param src_head first node on source list
 * @param cmp_head comparison list head
 * @param dest_head destination list head
 * @param dest_tail destination list tail
 *
 * @return node_t* new destination list tail
 **********************************************************/
node_t* list_repeat_copy(node_t* src_head, node_t* cmp_head, node_t* dest_head, node_t* dest_tail)
{
  if (dest_tail != NULL && cmp_head != NULL) {
    while (src_head != NULL) {
      if (list_find_data(cmp_head, &src_head->data) != NULL &&
          list_find_data(dest_head, &src_head->data) == NULL) {
        if (list_push(src_head, dest_tail))
          dest_tail = dest_tail->next;
      }

      src_head = src_head->next;
    }
  }

  return dest_tail;
}

/**********************************************************
 * Make union between two lists.
 *
 * @param first_head first list head
 * @param second_head second list head
 * @param out_head reference to pointer where resulting
 *                 list head will be stored
 **********************************************************/
void list_union(node_t* first_head, node_t* second_head, node_t** out_head)
{
  if (first_head != NULL && second_head != NULL && out_head != NULL) {
    if (node_alloc(out_head)) {
      /* initialize result list */
      data_copy(&first_head->data, &(*out_head)->data);

      node_t* tail = list_single_copy(first_head->next, *out_head, *out_head, *out_head);

      list_single_copy(second_head, *out_head, *out_head, tail);
    }
  }
}

/**********************************************************
 * Make intersection between two lists.
 *
 * @param first_head first list head
 * @param second_head second list head
 * @param out_head reference to pointer where resulting
 *                 list head will be stored
 **********************************************************/
void list_intersection(node_t* first_head, node_t* second_head, node_t** out_head)
{
  if (first_head != NULL && second_head != NULL && out_head != NULL) {
    if (node_alloc(out_head))
      list_repeat_copy(first_head, second_head, *out_head, *out_head);
  }
}

/**********************************************************
 * Make symmetric difference between two lists.
 *
 * @param first_head first list head
 * @param second_head second list head
 * @param out_head reference to pointer where resulting
 *                 list head will be stored
 **********************************************************/
void list_difference(node_t* first_head, node_t* second_head, node_t** out_head)
{
  if (first_head != NULL && second_head != NULL && out_head != NULL) {
    if (node_alloc(out_head)) {
      node_t* tail = list_single_copy(first_head, second_head, *out_head, *out_head);

      list_single_copy(second_head, first_head, *out_head, tail);
    }
  }
}

/**********************************************************
 * Verify if second list is subset of first list.
 *
 * @param first_head first list head
 * @param second_head second list head
 *
 * @return bool whether second list is or not subset of first
 *              list
 **********************************************************/
bool list_subset(node_t* first_head, node_t* second_head)
{
  if (first_head != NULL && second_head != NULL) {
    while (second_head != NULL) {
      if (list_find_data(first_head, &second_head->data) == NULL)
        return false;

      second_head = second_head->next;
    }
  }

  return true;
}

/**********************************************************
 * Free all nodes on list.
 *
 * @param head first node on list
 **********************************************************/
void list_free(node_t* head)
{
  if (head != NULL) {
    if (head->next == NULL)
      node_free(head);
    else {
      node_t* current = head->next;
      node_t* prev = head;

      while (current != NULL) {
        node_free(current->prev);

        prev = current;
        current = current->next;
      }

      node_free(prev);
    }
  }
}

#ifdef __cplusplus
}
#endif

#endif /* _list_h_ */
