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
#include <string.h>
#include "list.h"

void print_list(node_t* head)
{
  while (head != NULL) {
    if (head->data.address != NULL) {
      printf("\nNode:\n");

      for (size_t i = 0; i < head->data.size; i++)
        printf("%c", *(char*)(head->data.address + i));

      printf("\n");
    }

    head = head->next;
  }
}

bool string_split(const char* str, const size_t size, const char divider, node_t** out_head)
{
  if (str != NULL && size != 0 && out_head != NULL) {
    if (node_alloc(out_head)) {
      node_t* current = *out_head;
      size_t count = 0;
      int last_div = -1;

      do {
        if (str[count] == divider) {
          size_t substr_size = (size_t)((int)count - last_div) - 1;
          if (data_alloc(&current->data, substr_size + sizeof(char))) {
            memcpy(current->data.address, &str[last_div + 1], substr_size);
            *(char*)(current->data.address + substr_size) = '\0';
          }

          if (node_alloc(&current->next))
            current = current->next;

          last_div = (int)count;
        }
        else if (count == (size - 1)) {
          size_t substr_size = (size_t)((int)count - last_div);
          if (data_alloc(&current->data, substr_size + sizeof(char))) {
            memcpy(current->data.address, &str[last_div + 1], substr_size);
            *(char*)(current->data.address + substr_size) = '\0';
          }
        }

        count++;
      } while (count < size);

      return true;
    }
  }

  return false;
}

int main()
{
  node_t* head = NULL;

  const char* str = "Eu so quero ferias mano";
  const char div = ' ';

  if (string_split(str, strlen(str), div, &head))
    print_list(head);

  list_free(head);

  exit(EXIT_SUCCESS);
}
