/*
 * aJson
 * streamhelper.c
 *
 *  http://interactive-matter.org/
 *
 *  This file is part of aJson.
 *
 *  aJson is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  aJson is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with aJson.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Created on: 10.10.2010
 *      Author: marcus
 */
#include <stdio.h>
#include <stdlib.h>
#include "streamhelper.h"

//Default buffer (grow) size for strings
#define BUFFER_DEFAULT_SIZE 4

//internal prototypes
int
stringGet(FILE* stream);
int
stringPut(char c, FILE* stream);

typedef struct
{
  char* string;
  unsigned int position;
} string_input_stream_info;

typedef struct
{
  char* string;
  unsigned int position;
  unsigned int length;
} string_output_stream_info;

FILE*
openStringInputStream(char* string)
{
  FILE* result = fdevopen(NULL, &stringGet);
  if (result == NULL)
    {
      return NULL;
    }
  string_input_stream_info* udata = malloc(sizeof(string_input_stream_info));
  if (udata != NULL)
    {
      udata->string = string;
      udata->position = 0;
      fdev_set_udata(result,udata);
    }
  else
    {
      fclose(result);
      return NULL;
    }
  return result;
}

FILE*
openStringOutputStream(void)
{
  FILE* result = fdevopen(&stringPut, NULL);
  if (result == NULL)
    {
      return NULL;
    }
  string_output_stream_info* udata = malloc(sizeof(string_output_stream_info));
  if (udata == NULL)
    {
      fclose(result);
      return NULL;
    }
  char* initial_string = malloc(BUFFER_DEFAULT_SIZE * sizeof(char));
  if (initial_string == NULL)
    {
      free(udata);
      fclose(result);
      return NULL;
    }
  udata->string = initial_string;
  udata->length = BUFFER_DEFAULT_SIZE;
  udata->position = 0;
  fdev_set_udata(result,udata);
  return result;
}

char*
closeStringOutputStream(FILE* stream)
{
	//write a 0 to the end - that is how a string looks like
	fputc(0,stream);
  string_output_stream_info* udata =
      (string_output_stream_info*) fdev_get_udata(stream);
  char* result = udata->string;
  if (result == NULL)
    {
      fclose(stream);
      return NULL;
    }
  udata->string = NULL;
  result = realloc(result, udata->length * sizeof(char));
  fclose(stream);
  return result;
}

int
stringGet(FILE* stream)
{
  string_input_stream_info* udata =
      (string_input_stream_info*) fdev_get_udata(stream);
  char result = udata->string[udata->position];
  if (result == 0)
    {
      return EOF;
    }
  else
    {
      udata->position++;
      return result;
    }
}

int
stringPut(char c, FILE* stream)
{
  string_output_stream_info* udata =
      (string_output_stream_info*) fdev_get_udata(stream);
  char* string = addToBuffer(c, udata->string, &(udata->length),
      &(udata->position));
  if (string == NULL)
    {
      return EOF;
    }
  udata->string = string;
  return 0;
}

char*
addToBuffer(char value, char* buffer, unsigned int* buffer_length,
    unsigned int* buffer_bytes)
{
  if (((*buffer_bytes) + 1) >= (*buffer_length))
    {
      buffer = (char*) realloc((void*) buffer, ((*buffer_length)
          + BUFFER_DEFAULT_SIZE) * sizeof(char));
      if (buffer == NULL)
        {
          return NULL;
        }
      (*buffer_length) += BUFFER_DEFAULT_SIZE;
    }
	buffer[*buffer_bytes] = value;
	(*buffer_bytes) += 1;
	return buffer;
}

