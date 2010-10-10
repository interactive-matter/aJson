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

//internal prototypes
int
stringGet(FILE* stream);

typedef struct
{
  char* string;
  unsigned int position;
} string_stream_info;

FILE*
openStringInputStream(char* string)
{
  FILE* result = fdevopen(NULL, &stringGet);
  if (result == NULL)
    {
      return NULL;
    }
  string_stream_info* udata = malloc(sizeof(string_stream_info));
  if (udata != NULL)
    {
      udata->string = string;
      udata->position = 0;
      fdev_set_udata(result,udata);
    }
  else
    {
      free(result);
      return NULL;
    }
  return result;
}

int
stringGet(FILE* stream)
{
  string_stream_info* udata = (string_stream_info*) fdev_get_udata(stream);
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
