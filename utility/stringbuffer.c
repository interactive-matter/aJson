/*
 * aJson
 * stringbuffer.c
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
 *  Created on: 14.10.2010
 *      Author: marcus
 */
#include <stdlib.h>
#include "stringbuffer.h"

//Default buffer (grow) size for strings
#define BUFFER_DEFAULT_SIZE 4

string_buffer* stringBufferCreate(void) {
  string_buffer* result = malloc(sizeof(string_buffer));
  if (result==NULL) {
      return NULL;
  }
  result->string = malloc(BUFFER_DEFAULT_SIZE*sizeof(char));
  if (result->string==NULL) {
      free(result);
      return NULL;
  }
  result->memory=BUFFER_DEFAULT_SIZE;
  result->string_length=0;
  return result;
}

char
stringBufferAdd(char value, string_buffer* buffer)
{
  if (buffer->string_length + 1 >= buffer->memory)
    {
      buffer->string = (char*) realloc((void*) buffer->string, (buffer->memory
          + BUFFER_DEFAULT_SIZE) * sizeof(char));
      if (buffer->string == NULL)
        {
          return -1;
        }
      buffer->memory += BUFFER_DEFAULT_SIZE;
    }
  buffer->string[buffer->string_length] = value;
  buffer->string_length += 1;
  return 0;
}

char* stringBufferToString(string_buffer* buffer) {
  char* result = buffer->string;
  //ensure that the string ends with 0
  if (buffer->string_length==0 || buffer->string[buffer->string_length-1]!=0) {
      stringBufferAdd(0,buffer);
  }
  result = realloc(result, buffer->memory);
  free(buffer);
  return result;
}
