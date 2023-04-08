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
#include <string.h>
#include "stringbuffer.h"
#include <Arduino.h>
//#include <aJSON.h>


//Default buffer size for strings
#if defined(ARDUINO_ARCH_AVR)
#define BUFFER_SIZE 64
#else
#define BUFFER_SIZE 256
#endif
//there is a static buffer allocated, which is used to decode strings to
//strings cannot be longer than the buffer
char global_buffer[BUFFER_SIZE];

string_buffer*
stringBufferCreate(void)
{
  string_buffer* result = malloc(sizeof(string_buffer));
  if (result == NULL)
    {
      return NULL;
    }
  result->string = global_buffer;
  memset((void*) global_buffer, 0, BUFFER_SIZE);
  //unused - but will be usefull after realloc got fixd
  /*  if (result->string==NULL) {
   free(result);
   return NULL;
   }
   result->memory=BUFFER_DEFAULT_SIZE;*/
  result->memory = BUFFER_SIZE;
  result->string_length = 0;
  return result;
}

char
stringBufferAdd(char value, string_buffer* buffer)
{
  if (buffer->string_length >= buffer->memory)
    {
      //this has to be enabled after realloc works
      /*char* new_string = (char*) realloc((void*) buffer->string, (buffer->memory
       + BUFFER_DEFAULT_SIZE) * sizeof(char));
       if (new_string == NULL)
       {
       free(buffer->string);
       buffer->string = NULL;
       return -1;
       } else {
       buffer->string = new_string;
       }
       buffer->memory += BUFFER_DEFAULT_SIZE;*/
      //in the meantime we just drop it
      return 0; //EOF would be a better choice - but that breaks json decoding
    }
  buffer->string[buffer->string_length] = value;
  buffer->string_length += 1;
  return 0;
}

char*
stringBufferToString(string_buffer* buffer)
{
  //this is the realloc dependent function - it does not work
  //  char* result = buffer->string;
  //ensure that the string ends with 0
  if (buffer->string_length == 0 || buffer->string[(buffer->string_length - 1)]
      != 0)
    {
      stringBufferAdd(0, buffer);
    }
  /*  char* string = realloc(result, buffer->string_length);
   if (string==NULL) {
   free(result);
   }
   buffer->string=NULL;
   free(buffer);
   return string;*/

  ///char* result = malloc(buffer->string_length * sizeof(char));
  
  char * result = newString(global_buffer);
  if (result == NULL)
    {
      return NULL;
    }

  ///strcpy(result, global_buffer);

 

  buffer->string = NULL;
  free(buffer);
  return result;
}

void
stringBufferFree(string_buffer* buffer)
{
  if (buffer == NULL)
    {
      //hmm it was null before - whatever
      return;
    }
  //this is not needed in this realloc free concept
  /*
   if (buffer->string!=NULL) {
   free(buffer->string);
   }
   */
  free(buffer);
}

static string_card stringLib ={NULL,0,NULL}; 

  string_card *findString(char * str)
  {
    string_card * card = &stringLib;
    if (!str) return NULL;
    do
    {
       if (card->string && !strcmp(str,card->string)) return card;
       card = card->next;
    } while (card);
    return NULL;
  }

  char *newString(const char * str)
  { 
    //debugPrint(str);
     string_card * card = findString(str);
     string_card * prevCard;
     if (card) 
              { 
            //    debugPrint(":reused; ");
                card->used++;
                return card->string;
              }
     else
              {
                card = &stringLib;
                do
                {
                  if(!card->string)
                            {
                              card->string = strdup(str);
                              card->used=1;
              //                debugPrint(":added/replaced; ");
                              return card->string;
                            }
                prevCard = card;             
                card = card->next;
                } while (card);

                card = malloc (sizeof(string_card));
                if (card)
                {
                    prevCard->next = card;
                    card->string = strdup(str);
                    card->used=1;
                    card->next=NULL;
   //                 debugPrint(":added ");
                    return card->string;  
                }            
              }
  return NULL;                     
  }

  void  freeString(char * str)
  {
    string_card * card = findString(str);
    if (!card) return;
    card->used--;
    if (!card->used) 
                        {
                 //       debugPrint(str);  
                        free(card->string);
                        card->string = NULL;
                //        debugPrint(":removed ");
                        compressList();
                        }
  } 

  void compressList()
  {
  string_card * prevPtr = &stringLib;
  string_card * nextPtr = prevPtr->next; 
  while (nextPtr)
  {
   if (!nextPtr->string) 
          {
            //removing card
            prevPtr->next=nextPtr->next;
            free (nextPtr);
            nextPtr=prevPtr->next;
          }
    else 
          {
          prevPtr=nextPtr;  
          nextPtr=nextPtr->next;      
          }
  }
  }