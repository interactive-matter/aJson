/*
 Copyright (c) 2001, Interactive Matter, Marcus Nowotny

 Based on the cJSON Library, Copyright (C) 2009 Dave Gamble

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

// cJSON
// JSON parser in C.

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include <avr/pgmspace.h>
#include "aJSON.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

//the string constants to remove them from memory
static const prog_char* PROGMEM integer_format_P  = "%d";
static const prog_char* PROGMEM double_null_format_P = "%.0f";
static const prog_char* PROGMEM double_e_format_P = "%e";
static const prog_char* PROGMEM double_format_P = "%f";


// Internal constructor.
aJSON_Object* aJsonClass::new_Item() {
  aJSON_Object* node = (aJSON_Object*) malloc(sizeof(aJSON_Object));
  if (node)
    memset(node, 0, sizeof(aJSON_Object));
  return node;
}

// Delete a aJSON_Object structure.
void aJsonClass::delete_Item(aJSON_Object *c)
{
  aJSON_Object *next;
  while (c)
    {
      next = c->next;
      if (!(c->type & cJSON_IsReference) && c->child)
        delete_Item(c->child);
      if (!(c->type & cJSON_IsReference) && c->value.valuestring)
        free(c->value.valuestring);
      if (c->string)
        free(c->string);
      free(c);
      c = next;
    }
}

// Parse the input text to generate a number, and populate the result into item.
const char* aJsonClass::parse_number(aJSON_Object *item, const char *num)
{
  float n = 0, sign = 1, scale = 0;
  int subscale = 0, signsubscale = 1;

  // TODO Could use sscanf for this?
  if (*num == '-')
    sign = -1, num++; // Has sign?
  if (*num == '0')
    num++; // is zero
  if (*num >= '1' && *num <= '9')
    do
      n = (n * 10.0) + (*num++ - '0');
    while (*num >= '0' && *num <= '9'); // Number?
  if (*num == '.')
    {
      num++;
      do
        n = (n * 10.0) + (*num++ - '0'), scale--;
      while (*num >= '0' && *num <= '9');
    } // Fractional part?
  if (*num == 'e' || *num == 'E') // Exponent?
    {
      num++;
      if (*num == '+')
        num++;
      else if (*num == '-')
        signsubscale = -1, num++; // With sign?
      while (*num >= '0' && *num <= '9')
        subscale = (subscale * 10) + (*num++ - '0'); // Number?
    }

  n = sign * n * pow(10.0, (scale + subscale * signsubscale)); // number = +/- number.fraction * 10^+/- exponent

  item->value.number.valuedouble = n;
  item->value.number.valueint = (int) n;
  item->type = cJSON_Number;
  return num;
}

// Render the number nicely from the given item into a string.
char* aJsonClass::print_number(aJSON_Object *item)
{
  char *str;
  float d = item->value.number.valuedouble;
  if (fabs(((float) item->value.number.valueint) - d) <= FLT_EPSILON && d
      <= INT_MAX && d >= INT_MIN)
    {
      str = (char*) malloc(21); // 2^64+1 can be represented in 21 chars.
      if (str)
        sprintf_P(str, integer_format_P, item->value.number.valueint);
    }
  else
    {
      str = (char*) malloc(64); // This is a nice tradeoff.
      if (str)
        {
          if (fabs(floor(d) - d) <= DBL_EPSILON)
            sprintf_P(str, double_null_format_P, d);
          else if (fabs(d) < 1.0e-6 || fabs(d) > 1.0e9)
            sprintf_P(str, double_e_format_P, d);
          else
            sprintf_P(str, double_format_P, d);
        }
    }
  return str;
}

// Parse the input text into an unescaped cstring, and populate item.
static const unsigned char firstByteMark[7] =
  { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

const char* aJsonClass::parse_string(aJSON_Object *item, const char *str)
{
  const char *ptr = str + 1;
  char *ptr2;
  char *out;
  int len = 0;

  if (*str != '\"')
    return 0; // not a string!

  while (*ptr != '\"' && (unsigned char) *ptr > 31 && ++len)
    if (*ptr++ == '\\')
      ptr++; // Skip escaped quotes.

  out = (char*) malloc(len + 1); // This is how long we need for the string, roughly.
  if (!out)
    return 0;

  ptr = str + 1;
  ptr2 = out;
  while (*ptr != '\"' && (unsigned char) *ptr > 31)
    {
      if (*ptr != '\\')
        *ptr2++ = *ptr++;
      else
        {
          ptr++;
          switch (*ptr)
            {
          case 'b':
            *ptr2++ = '\b';
            break;
          case 'f':
            *ptr2++ = '\f';
            break;
          case 'n':
            *ptr2++ = '\n';
            break;
          case 'r':
            *ptr2++ = '\r';
            break;
          case 't':
            *ptr2++ = '\t';
            break;
            //no unicode support
            /*          case 'u': // transcode utf16 to utf8. DOES NOT SUPPORT SURROGATE PAIRS CORRECTLY.
             sscanf(ptr + 1, "%4x", &uc); // get the unicode char.
             len = 3;
             if (uc < 0x80)
             len = 1;
             else if (uc < 0x800)
             len = 2;
             ptr2 += len;

             switch (len)
             {
             case 3:
             *--ptr2 = ((uc | 0x80) & 0xBF);
             uc >>= 6;
             case 2:
             *--ptr2 = ((uc | 0x80) & 0xBF);
             uc >>= 6;
             case 1:
             *--ptr2 = (uc | firstByteMark[len]);
             }
             ptr2 += len;
             ptr += 4;
             break;
             */
          default:
            *ptr2++ = *ptr;
            break;
            }
          ptr++;
        }
    }
  *ptr2 = 0;
  if (*ptr == '\"')
    ptr++;
  item->value.valuestring = out;
  item->type = cJSON_String;
  return ptr;
}

// Render the cstring provided to an escaped version that can be printed.
char* aJsonClass::print_string_ptr(const char *str)
{
  const char *ptr;
  char *ptr2, *out;
  int len = 0;

  if (!str)
    return strdup("");
  ptr = str;
  while (*ptr && ++len)
    {
      if ((unsigned char) *ptr < 32 || *ptr == '\"' || *ptr == '\\')
        len++;
      ptr++;
    }

  out = (char*) malloc(len + 3);
  if (!out)
    return 0;

  ptr2 = out;
  ptr = str;
  *ptr2++ = '\"';
  while (*ptr)
    {
      if ((unsigned char) *ptr > 31 && *ptr != '\"' && *ptr != '\\')
        *ptr2++ = *ptr++;
      else
        {
          *ptr2++ = '\\';
          switch (*ptr++)
            {
          case '\\':
            *ptr2++ = '\\';
            break;
          case '\"':
            *ptr2++ = '\"';
            break;
          case '\b':
            *ptr2++ = 'b';
            break;
          case '\f':
            *ptr2++ = 'f';
            break;
          case '\n':
            *ptr2++ = 'n';
            break;
          case '\r':
            *ptr2++ = 'r';
            break;
          case '\t':
            *ptr2++ = 't';
            break;
          default:
            ptr2--;
            break; // eviscerate with prejudice.
            }
        }
    }
  *ptr2++ = '\"';
  *ptr2++ = 0;
  return out;
}

// Invote print_string_ptr (which is useful) on an item.
char* aJsonClass::print_string(aJSON_Object *item)
{
  return print_string_ptr(item->value.valuestring);
}

// Utility to jump whitespace and cr/lf
const char* aJsonClass::skip(const char *in)
{
  while (in && (unsigned char) *in <= 32)
    in++;
  return in;
}

// Parse an object - create a new root, and populate.
aJSON_Object* aJsonClass::parse(const char *value)
{
  aJSON_Object *c = new_Item();
  if (!c)
    return 0; /* memory fail */

  if (!parse_value(c, skip(value)))
    {
      delete_Item(c);
      return 0;
    }
  return c;
}

// Render a aJSON_Object item/entity/structure to text.
char* aJsonClass::print(aJSON_Object *item)
{
  return print_value(item, 0, 1);
}
char* aJsonClass::printUnformatted(aJSON_Object *item)
{
  return print_value(item, 0, 0);
}

// Parser core - when encountering text, process appropriately.
const char* aJsonClass::parse_value(aJSON_Object *item, const char *value)
{
  if (!value)
    return 0; // Fail on null.
  if (!strncmp(value, "null", 4))
    {
      item->type = cJSON_NULL;
      return value + 4;
    }
  if (!strncmp(value, "false", 5))
    {
      item->type = cJSON_False;
      item->value.valuebool = -1;
      return value + 5;
    }
  if (!strncmp(value, "true", 4))
    {
      item->type = cJSON_True;
      item->value.valuebool = 1;
      return value + 4;
    }
  if (*value == '\"')
    {
      return parse_string(item, value);
    }
  if (*value == '-' || (*value >= '0' && *value <= '9'))
    {
      return parse_number(item, value);
    }
  if (*value == '[')
    {
      return parse_array(item, value);
    }
  if (*value == '{')
    {
      return parse_object(item, value);
    }

  return 0; // failure.
}

// Render a value to text.
char* aJsonClass::print_value(aJSON_Object *item, unsigned char depth, char fmt)
{
  char *out = 0;
  if (!item)
    return 0;
  switch ((item->type) & 255)
    {
  case cJSON_NULL:
    out = strdup("null");
    break;
  case cJSON_False:
    out = strdup("false");
    break;
  case cJSON_True:
    out = strdup("true");
    break;
  case cJSON_Number:
    out = print_number(item);
    break;
  case cJSON_String:
    out = print_string(item);
    break;
  case cJSON_Array:
    out = print_array(item, depth, fmt);
    break;
  case cJSON_Object:
    out = print_object(item, depth, fmt);
    break;
    }
  return out;
}

// Build an array from input text.
const char* aJsonClass::parse_array(aJSON_Object *item, const char *value)
{
  aJSON_Object *child;
  if (*value != '[')
    return 0; // not an array!

  item->type = cJSON_Array;
  value = skip(value + 1);
  if (*value == ']')
    return value + 1; // empty array.

  item->child = child = new_Item();
  if (!item->child)
    return 0; // memory fail
  value = skip(parse_value(child, skip(value))); // skip any spacing, get the value.
  if (!value)
    return 0;

  while (*value == ',')
    {
      aJSON_Object *new_item;
      if (!(new_item = new_Item()))
        return 0; // memory fail
      child->next = new_item;
      new_item->prev = child;
      child = new_item;
      value = skip(parse_value(child, skip(value + 1)));
      if (!value)
        return 0; // memory fail
    }

  if (*value == ']')
    return value + 1; // end of array
  return 0; // malformed.
}

// Render an array to text
char* aJsonClass::print_array(aJSON_Object *item, unsigned char depth, char fmt)
{
  char **entries;
  char *out = 0, *ptr, *ret;
  int len = 5;
  aJSON_Object *child = item->child;
  unsigned char numentries = 0, i = 0, fail = 0;

  // How many entries in the array?
  while (child)
    numentries++, child = child->next;
  // Allocate an array to hold the values for each
  entries = (char**) malloc(numentries * sizeof(char*));
  if (!entries)
    return 0;
  memset(entries, 0, numentries * sizeof(char*));
  // Retrieve all the results:
  child = item->child;
  while (child && !fail)
    {
      ret = print_value(child, depth + 1, fmt);
      entries[i++] = ret;
      if (ret)
        len += strlen(ret) + 2 + (fmt ? 1 : 0);
      else
        fail = 1;
      child = child->next;
    }

  // If we didn't fail, try to malloc the output string
  if (!fail)
    out = (char*) malloc(len);
  // If that fails, we fail.
  if (!out)
    fail = 1;

  // Handle failure.
  if (fail)
    {
      for (i = 0; i < numentries; i++)
        if (entries[i])
          free(entries[i]);
      free(entries);
      return 0;
    }

  // Compose the output array.
  *out = '[';
  ptr = out + 1;
  *ptr = 0;
  for (i = 0; i < numentries; i++)
    {
      strcpy(ptr, entries[i]);
      ptr += strlen(entries[i]);
      if (i != numentries - 1)
        {
          *ptr++ = ',';
          if (fmt)
            *ptr++ = ' ';
          *ptr = 0;
        }
      free(entries[i]);
    }
  free(entries);
  *ptr++ = ']';
  *ptr++ = 0;
  return out;
}

// Build an object from the text.
const char* aJsonClass::parse_object(aJSON_Object *item, const char *value)
{
  aJSON_Object *child;
  if (*value != '{')
    return NULL; // not an object!

  item->type = cJSON_Object;
  value = skip(value + 1);
  if (*value == '}')
    return value + 1; // empty array.

  item->child = child = new_Item();
  if (!item->child)
    return NULL;
  value = skip(parse_string(child, skip(value)));
  if (!value)
    return NULL;
  child->string = child->value.valuestring;
  child->value.valuestring = NULL;
  if (*value != ':')
    return NULL; // fail!
  value = skip(parse_value(child, skip(value + 1))); // skip any spacing, get the value.
  if (!value)
    return NULL;

  while (*value == ',')
    {
      aJSON_Object *new_item;
      if (!(new_item = new_Item()))
        return NULL; // memory fail
      child->next = new_item;
      new_item->prev = child;
      child = new_item;
      value = skip(parse_string(child, skip(value + 1)));
      if (!value)
        return NULL;
      child->string = child->value.valuestring;
      child->value.valuestring = NULL;
      if (*value != ':')
        return NULL; // fail!
      value = skip(parse_value(child, skip(value + 1))); // skip any spacing, get the value.
      if (!value)
        return NULL;
    }

  if (*value == '}')
    return value + 1; // end of array
  return NULL; // malformed.
}

// Render an object to text.
char* aJsonClass::print_object(aJSON_Object *item, unsigned char depth, char fmt)
{
  char **entries = 0, **names = 0;
  char *out = 0, *ptr, *ret, *str;
  int len = 7;
  unsigned char i = 0, j;
  aJSON_Object *child = item->child;
  int numentries = 0, fail = 0;
  // Count the number of entries.
  while (child)
    numentries++, child = child->next;
  // Allocate space for the names and the objects
  entries = (char**) malloc(numentries * sizeof(char*));
  if (!entries)
    return 0;
  names = (char**) malloc(numentries * sizeof(char*));
  if (!names)
    {
      free(entries);
      return 0;
    }
  memset(entries, 0, sizeof(char*) * numentries);
  memset(names, 0, sizeof(char*) * numentries);

  // Collect all the results into our arrays:
  child = item->child;
  depth++;
  if (fmt)
    len += depth;
  while (child)
    {
      names[i] = str = print_string_ptr(child->string);
      entries[i++] = ret = print_value(child, depth, fmt);
      if (str && ret)
        len += strlen(ret) + strlen(str) + 2 + (fmt ? 2 + depth : 0);
      else
        fail = 1;
      child = child->next;
    }

  // Try to allocate the output string
  if (!fail)
    out = (char*) malloc(len);
  if (!out)
    fail = 1;

  // Handle failure
  if (fail)
    {
      for (i = 0; i < numentries; i++)
        {
          if (names[i])
            free(names[i]);
          if (entries[i])
            free(entries[i]);
        }
      free(names);
      free(entries);
      return 0;
    }

  // Compose the output:
  *out = '{';
  ptr = out + 1;
  if (fmt)
    *ptr++ = '\n';
  *ptr = 0;
  for (i = 0; i < numentries; i++)
    {
      if (fmt)
        for (j = 0; j < depth; j++)
          *ptr++ = '\t';
      strcpy(ptr, names[i]);
      ptr += strlen(names[i]);
      *ptr++ = ':';
      if (fmt)
        *ptr++ = '\t';
      strcpy(ptr, entries[i]);
      ptr += strlen(entries[i]);
      if (i != numentries - 1)
        *ptr++ = ',';
      if (fmt)
        *ptr++ = '\n';
      *ptr = 0;
      free(names[i]);
      free(entries[i]);
    }

  free(names);
  free(entries);
  if (fmt)
    for (i = 0; i < depth - 1; i++)
      *ptr++ = '\t';
  *ptr++ = '}';
  *ptr++ = 0;
  return out;
}

// Get Array size/item / object item.
unsigned char aJsonClass::getArraySize(aJSON_Object *array)
{
  aJSON_Object *c = array->child;
  unsigned char i = 0;
  while (c)
    i++, c = c->next;
  return i;
}
aJSON_Object* aJsonClass::getArrayItem(aJSON_Object *array, unsigned char item)
{
  aJSON_Object *c = array->child;
  while (c && item > 0)
    item--, c = c->next;
  return c;
}
aJSON_Object* aJsonClass::getObjectItem(aJSON_Object *object, const char *string)
{
  aJSON_Object *c = object->child;
  while (c && strcmp(c->string, string))
    c = c->next;
  return c;
}

// Utility for array list handling.
void aJsonClass::suffix_object(aJSON_Object *prev, aJSON_Object *item)
{
  prev->next = item;
  item->prev = prev;
}
// Utility for handling references.
aJSON_Object* aJsonClass::create_reference(aJSON_Object *item)
{
  aJSON_Object *ref = new_Item();
  if (!ref)
    return 0;
  memcpy(ref, item, sizeof(aJSON_Object));
  ref->string = 0;
  ref->type |= cJSON_IsReference;
  ref->next = ref->prev = 0;
  return ref;
}

// Add item to array/object.
void aJsonClass::addItemToArray(aJSON_Object *array, aJSON_Object *item)
{
  aJSON_Object *c = array->child;
  if (!item)
    return;
  if (!c)
    {
      array->child = item;
    }
  else
    {
      while (c && c->next)
        c = c->next;
      suffix_object(c, item);
    }
}
void aJsonClass::addItemToObject(aJSON_Object *object, const char *string, aJSON_Object *item)
{
  if (!item)
    return;
  if (item->string)
    free(item->string);
  item->string = strdup(string);
  addItemToArray(object, item);
}
void aJsonClass::addItemReferenceToArray(aJSON_Object *array, aJSON_Object *item)
{
  addItemToArray(array, create_reference(item));
}
void aJsonClass::addItemReferenceToObject(aJSON_Object *object, const char *string, aJSON_Object *item)
{
  addItemToObject(object, string, create_reference(item));
}

aJSON_Object* aJsonClass::detachItemFromArray(aJSON_Object *array, unsigned char which)
{
  aJSON_Object *c = array->child;
  while (c && which > 0)
    c = c->next, which--;
  if (!c)
    return 0;
  if (c->prev)
    c->prev->next = c->next;
  if (c->next)
    c->next->prev = c->prev;
  if (c == array->child)
    array->child = c->next;
  c->prev = c->next = 0;
  return c;
}
void aJsonClass::deleteItemFromArray(aJSON_Object *array, unsigned char which)
{
  delete_Item(detachItemFromArray(array, which));
}
aJSON_Object* aJsonClass::detachItemFromObject(aJSON_Object *object, const char *string)
{
  unsigned char i = 0;
  aJSON_Object *c = object->child;
  while (c && strcmp(c->string, string))
    i++, c = c->next;
  if (c)
    return detachItemFromArray(object, i);
  return 0;
}
void aJsonClass::deleteItemFromObject(aJSON_Object *object, const char *string)
{
  delete_Item(detachItemFromObject(object, string));
}

// Replace array/object items with new ones.
void aJsonClass::replaceItemInArray(aJSON_Object *array, unsigned char which, aJSON_Object *newitem)
{
  aJSON_Object *c = array->child;
  while (c && which > 0)
    c = c->next, which--;
  if (!c)
    return;
  newitem->next = c->next;
  newitem->prev = c->prev;
  if (newitem->next)
    newitem->next->prev = newitem;
  if (c == array->child)
    array->child = newitem;
  else
    newitem->prev->next = newitem;
  c->next = c->prev = 0;
  delete_Item(c);
}
void aJsonClass::replaceItemInObject(aJSON_Object *object, const char *string, aJSON_Object *newitem)
{
  unsigned char i = 0;
  aJSON_Object *c = object->child;
  while (c && strcmp(c->string, string))
    i++, c = c->next;
  if (c)
    {
      newitem->string = strdup(string);
      replaceItemInArray(object, i, newitem);
    }
}

// Create basic types:
aJSON_Object* aJsonClass::createNull()
{
  aJSON_Object *item = new_Item();
  if (item)
    item->type = cJSON_NULL;
  return item;
}

aJSON_Object* aJsonClass::createTrue()
{
  aJSON_Object *item = new_Item();
  if (item)
    {
      item->type = cJSON_True;
      item->value.valuebool = -1;
    }
  return item;
}
aJSON_Object* aJsonClass::createFalse()
{
  aJSON_Object *item = new_Item();
  if (item)
    {
      item->type = cJSON_False;
      item->value.valuebool = 0;
    }
  return item;
}
aJSON_Object* aJsonClass::createBool(char b)
{
  aJSON_Object *item = new_Item();
  if (item)
    {
      item->type = b ? cJSON_True : cJSON_False;
      item->value.valuebool = b ? -1 : 0;
    }
  return item;
}

aJSON_Object* aJsonClass::createNumber(float num)
{
  aJSON_Object *item = new_Item();
  if (item)
    {
      item->type = cJSON_Number;
      item->value.number.valuedouble = num;
      item->value.number.valueint = (int) num;
    }
  return item;
}

aJSON_Object* aJsonClass::createString(const char *string)
{
  aJSON_Object *item = new_Item();
  if (item)
    {
      item->type = cJSON_String;
      item->value.valuestring = strdup(string);
    }
  return item;
}

aJSON_Object* aJsonClass::createArray()
{
  aJSON_Object *item = new_Item();
  if (item)
    item->type = cJSON_Array;
  return item;
}
aJSON_Object* aJsonClass::createObject()
{
  aJSON_Object *item = new_Item();
  if (item)
    item->type = cJSON_Object;
  return item;
}

// Create Arrays:
aJSON_Object* aJsonClass::createIntArray(int *numbers, unsigned char count)
{
  unsigned char i;
  aJSON_Object *n = 0, *p = 0, *a = createArray();
  for (i = 0; a && i < count; i++)
    {
      n = createNumber(numbers[i]);
      if (!i)
        a->child = n;
      else
        suffix_object(p, n);
      p = n;
    }
  return a;
}

aJSON_Object* aJsonClass::createFloatArray(float *numbers, unsigned char count)
{
  unsigned char i;
  aJSON_Object *n = 0, *p = 0, *a = createArray();
  for (i = 0; a && i < count; i++)
    {
      n = createNumber(numbers[i]);
      if (!i)
        a->child = n;
      else
        suffix_object(p, n);
      p = n;
    }
  return a;
}

aJSON_Object* aJsonClass::createDoubleArray(float *numbers, unsigned char count)
{
  unsigned char i;
  aJSON_Object *n = 0, *p = 0, *a = createArray();
  for (i = 0; a && i < count; i++)
    {
      n = createNumber(numbers[i]);
      if (!i)
        a->child = n;
      else
        suffix_object(p, n);
      p = n;
    }
  return a;
}

aJSON_Object* aJsonClass::createStringArray(const char **strings, unsigned char count)
{
  unsigned char i;
  aJSON_Object *n = 0, *p = 0, *a = createArray();
  for (i = 0; a && i < count; i++)
    {
      n = createString(strings[i]);
      if (!i)
        a->child = n;
      else
        suffix_object(p, n);
      p = n;
    }
  return a;
}

void aJsonClass::addNullToObject(aJSON_Object* object, const char* name) {
	addItemToObject(object, name, createNull());
}

void aJsonClass::addTrueToObject(aJSON_Object* object,const char* name) {
	addItemToObject(object, name, createTrue());
}

void aJsonClass::addFalseToObject(aJSON_Object* object,const char* name) {
	addItemToObject(object, name, createFalse());
}

void aJsonClass::addNumberToObject(aJSON_Object* object,const char* name, int n) {
	addItemToObject(object, name, createNumber(n));
}

void aJsonClass::addStringToObject(aJSON_Object* object,const char* name, const char* s) {
	addItemToObject(object, name, createString(s));
}

aJsonClass aJson;
