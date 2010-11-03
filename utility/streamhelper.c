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
#include "stringbuffer.h"

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

void
closeStringInputStream(FILE* stream)
{
  if (stream == NULL)
    {
      return;
    }
  string_input_stream_info* udata =
      (string_input_stream_info*) fdev_get_udata(stream);
  if (udata != NULL)
    {
      free(udata);
    }
  fdev_set_udata(stream,NULL);
  fclose(stream);
}

FILE*
openStringOutputStream(void)
{
  FILE* result = fdevopen(&stringPut, NULL);
  if (result == NULL)
    {
      return NULL;
    }
  string_buffer* buffer = stringBufferCreate();
  if (buffer == NULL)
    {
      fclose(result);
      return NULL;
    }
  fdev_set_udata(result,buffer);
  return result;
}

char*
closeStringOutputStream(FILE* stream)
{
  //write a 0 to the end - that is how a string looks like
  string_buffer* buffer = (string_buffer*) fdev_get_udata(stream);
  char* result = stringBufferToString(buffer);
  if (result == NULL)
    {
      fclose(stream);
      return NULL;
    }
  //free(buffer);
  fdev_set_udata(stream,NULL);
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
  string_buffer* buffer = (string_buffer*) fdev_get_udata(stream);
  if (stringBufferAdd(c, buffer))
    {
      return EOF;
    }
  return 0;
}
