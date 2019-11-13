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
#include "file.h"

#define PERSON_NAME_SIZE 256        /* size of person's name */
#define FILE_NAME        "temp.dat" /* file to store data */
#define NUM_PEOPLE       10         /* how many people to generate */

typedef struct person_s {
  char name[PERSON_NAME_SIZE];
  int  age;
  int  num_cars;
} person_t;

void generate_random_bytes(char buffer[], size_t size)
{
  if (buffer != NULL) {
    srand(clock());

    for (size_t i = 0; i < size - 1; i++)
      buffer[i] = (char)(rand() % 90 + 65);

    buffer[size - 1] = '\0';
  }
}

int main()
{
  /* create and load file */
  file_create(FILE_NAME);

  file_t* file = NULL;

  if (file_open(&file, FILE_NAME)) {
    person_t* someone = malloc(sizeof(person_t));

    if (someone != NULL) {
      /* update file buffer */
      file->buffer.address = (ubyte_t*)someone;
      file->buffer.size = sizeof(person_t);

      for (int i = 0; i < NUM_PEOPLE; i++) {
        /* generate data */
        generate_random_bytes(someone->name, PERSON_NAME_SIZE);
        someone->age = (int)(rand() % 120 + 18);
        someone->num_cars = (int)(rand() % 10);

        /* write to file */
        file_write(file, FTELL(file->handle), file->buffer.size, true);
      }

      /* no need to free the buffer, it'll be reallocated later (or freed when calling file_close) */
    }

    /* read and print data */
    FSEEK(file->handle, 0, SEEK_SET);

    while (file_read(file, FTELL(file->handle), sizeof(person_t), true)) {
      if (file->buffer.size != sizeof(person_t))
        break;

      person_t* temp_ptr = (person_t*)file->buffer.address;

      printf("Name: %s\nAge: %i\nCars: %i\n", temp_ptr->name, temp_ptr->age, temp_ptr->num_cars);
    }

    file_close(file);
  }

  exit(EXIT_SUCCESS);
}
