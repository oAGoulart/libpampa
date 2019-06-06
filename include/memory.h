/*
 * Copyright 2019 Augusto Goulart
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

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "types.h"

/* define structs */
typedef struct data
{
	size_t size;    /* buffer size without ending nul char */
	UBYTE* address; /* data location */
} data_t;

/* free data buffer memory */
void data_free_buffer(data_t* data)
{
	if (data->address != NULL) {
		free(data->address);
		data->address = NULL;
	}

	data->size = 0;
}

/* allocate data buffer memory */
bool data_alloc_buffer(data_t* data)
{
	bool result = false;

	if (data != NULL) {
		/* allocate memory */
		data->address = (UBYTE*)realloc(data->address, (data->size + 1));

		if (data->address != NULL) {
			/* prevents memory overflow when using string.h functions */
			data->address[data->size] = '\0';

			result = true;
		}
	}

	return result;
}

#endif
