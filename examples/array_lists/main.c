/*
 * Copyright 2019 José Augusto dos Santos Goulart
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

#define ARRAY_SIZE 10

void print_list(node_t* head)
{
  while (head != NULL) {
    if (head->data.address != NULL) {
      printf("\nNode:\n");

      for (size_t i = 0; i < head->data.size; i++)
        printf("%u", *(ubyte_t*)(head->data.address + i));

      printf("\n");
    }

    head = head->next;
  }
}

bool generate_square_array(size_t size, ulong_t** out_array)
{
  if (out_array != NULL && size != 0) {
    for (size_t i = 0; i < size; i++) {
      for (size_t j = 0; j < size; j++)
        out_array[i][j] = rand();
    }

    return true;
  }

  return false;
}

bool partition_square_array(ulong_t** array, size_t size, node_t** out_head)
{
  if (array != NULL && out_head != NULL) {
    if (node_alloc(out_head)) {
      size_t count = 1;
      node_t* current = *out_head;
      size_t half_array = size / 2;
      size_t half_size = half_array * sizeof(array[0][0]);
      size_t line_size = size * sizeof(array[0][0]);

      if (size % 2 == 0) {
        do {
          current->data.size = half_size * half_size;

          if (data_alloc(&current->data)) {
            switch (count) {
              case 1:
                for (size_t i = 0; i < half_array; i++)
                  memcpy(current->data.address, &array[i], half_size);

                break;

              case 2:
                for (size_t i = 0; i < half_array; i++)
                  memcpy(current->data.address, &array[i] + half_size, half_size);

                break;

              case 3:
                for (size_t i = half_array; i < size; i++)
                  memcpy(current->data.address, &array[i], half_size);

                break;

              case 4:
                for (size_t i = half_array; i < size; i++)
                  memcpy(current->data.address, &array[i] + half_size, half_size);

                break;
                
              default:
                break;
            }
          }

          if (count < 4) {
            if (node_alloc(&current->next))
              current = current->next;
          }

          count++;
        } while (count <= 4);
      }
      else {
        do {
          switch (count) {
            case 1:
              current->data.size = (line_size - half_size) * (line_size - half_size);

              if (data_alloc(&current->data)) {
                for (size_t i = 0; i < (size - half_array); i++)
                  memcpy(current->data.address, &array[i], (line_size - half_size));
              }

              break;

            case 2:
              current->data.size = (line_size - half_size) * half_size;

              if (data_alloc(&current->data)) {
                for (size_t i = 0; i < (size - half_array); i++)
                  memcpy(current->data.address, &array[i] + (line_size - half_size), half_size);
              }

              break;

            case 3:
              current->data.size = half_size * (line_size - half_size);

              if (data_alloc(&current->data)) {
                for (size_t i = 0; i < half_array; i++)
                  memcpy(current->data.address, &array[i], (line_size - half_size));
              }

              break;

            case 4:
              current->data.size = half_size * half_size;

              if (data_alloc(&current->data)) {
                for (size_t i = 0; i < half_array; i++)
                  memcpy(current->data.address, &array[i] + (line_size - half_size), half_size);
              }

              break;
                
            default:
              break;
          }

          if (count < 4) {
            if (node_alloc(&current->next)) {
              current = current->next;
              count++;
            }
          }
        } while (count <= 4);
      }

      return true;
    }
  }

  return false;
}

int main()
{
  srand((unsigned int)time(NULL));

  node_t* head = NULL;

  ulong_t** array = malloc(ARRAY_SIZE * sizeof(*array));

  if (array != NULL) {
    for (size_t i = 0; i < ARRAY_SIZE; i++)
      array[i] = malloc(ARRAY_SIZE * sizeof(*array[0]));
  }

  if (generate_square_array(ARRAY_SIZE, array)) {
    if (partition_square_array(array, ARRAY_SIZE, &head))
      print_list(head);
  }

  exit(EXIT_SUCCESS);
}
