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
const prog_char PROGMEM integer_format_P[]  = "%d";
const prog_char PROGMEM double_null_format_P[] = "%.0f";
const prog_char PROGMEM double_e_format_P[] = "%e";
const prog_char PROGMEM double_format_P[] = "%f";


// Internal constructor.
aJsonObject* aJsonClass::newItem() {
  aJsonObject* node = (aJsonObject*) malloc(sizeof(aJsonObject));
  if (node)
    memset(node, 0, sizeof(aJsonObject));
  return node;
}

// Delete a aJsonObject structure.
void aJsonClass::deleteItem(aJsonObject *c)
{
  aJsonObject *next;
  while (c)
    {
      next = c->next;
      if (!(c->type & aJson_IsReference) && c->child)
        deleteItem(c->child);
      if (!(c->type & aJson_IsReference) && c->value.valuestring)
        free(c->value.valuestring);
      if (c->name)
        free(c->name);
      free(c);
      c = next;
    }
}

// Parse the input text to generate a number, and populate the result into item.
const char* aJsonClass::parseNumber(aJsonObject *item, const char *num)
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

  item->value.number.valuefloat = n;
  item->value.number.valueint = (int) n;
  item->type = aJson_Number;
  return num;
}

// Render the number nicely from the given item into a string.
char* aJsonClass::printNumber(aJsonObject *item)
{
  char *str;
  float d = item->value.number.valuefloat;
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

const char* aJsonClass::parseString(aJsonObject *item, const char *str)
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
  item->type = aJson_String;
  return ptr;
}

// Render the cstring provided to an escaped version that can be printed.
char* aJsonClass::printStringPtr(const char *str)
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
char* aJsonClass::printString(aJsonObject *item)
{
  return printStringPtr(item->value.valuestring);
}

// Utility to jump whitespace and cr/lf
const char* aJsonClass::skip(const char *in)
{
  while (in && (unsigned char) *in <= 32)
    in++;
  return in;
}

// Parse an object - create a new root, and populate.
aJsonObject* aJsonClass::parse(const char *value)
{
  aJsonObject *c = newItem();
  if (!c)
    return 0; /* memory fail */

  if (!parseValue(c, skip(value)))
    {
      deleteItem(c);
      return 0;
    }
  return c;
}

// Render a aJsonObject item/entity/structure to text.
char* aJsonClass::print(aJsonObject *item)
{
  return printValue(item);
}

// Parser core - when encountering text, process appropriately.
const char* aJsonClass::parseValue(aJsonObject *item, const char *value)
{
  if (!value)
    return 0; // Fail on null.
  if (!strncmp(value, "null", 4))
    {
      item->type = aJson_NULL;
      return value + 4;
    }
  if (!strncmp(value, "false", 5))
    {
      item->type = aJson_False;
      item->value.valuebool = -1;
      return value + 5;
    }
  if (!strncmp(value, "true", 4))
    {
      item->type = aJson_True;
      item->value.valuebool = 1;
      return value + 4;
    }
  if (*value == '\"')
    {
      return parseString(item, value);
    }
  if (*value == '-' || (*value >= '0' && *value <= '9'))
    {
      return parseNumber(item, value);
    }
  if (*value == '[')
    {
      return parseArray(item, value);
    }
  if (*value == '{')
    {
      return parseObject(item, value);
    }

  return 0; // failure.
}

// Render a value to text.
char* aJsonClass::printValue(aJsonObject *item)
{
  char *out = NULL;
  if (!item)
    return NULL;
  switch (item->type)
    {
  case aJson_NULL:
    out = strdup("null");
    break;
  case aJson_False:
    out = strdup("false");
    break;
  case aJson_True:
    out = strdup("true");
    break;
  case aJson_Number:
    out = printNumber(item);
    break;
  case aJson_String:
    out = printString(item);
    break;
  case aJson_Array:
    out = printArray(item);
    break;
  case aJson_Object:
    out = printObject(item);
    break;
    }
  return out;
}

// Build an array from input text.
const char* aJsonClass::parseArray(aJsonObject *item, const char *value)
{
  aJsonObject *child;
  if (*value != '[')
    return 0; // not an array!

  item->type = aJson_Array;
  value = skip(value + 1);
  if (*value == ']')
    return value + 1; // empty array.

  item->child = child = newItem();
  if (!item->child)
    return 0; // memory fail
  value = skip(parseValue(child, skip(value))); // skip any spacing, get the value.
  if (!value)
    return 0;

  while (*value == ',')
    {
      aJsonObject *new_item;
      if (!(new_item = newItem()))
        return 0; // memory fail
      child->next = new_item;
      new_item->prev = child;
      child = new_item;
      value = skip(parseValue(child, skip(value + 1)));
      if (!value)
        return 0; // memory fail
    }

  if (*value == ']')
    return value + 1; // end of array
  return 0; // malformed.
}

// Render an array to text
char* aJsonClass::printArray(aJsonObject *item)
{
  char **entries;
  char *out = 0, *ptr, *ret;
  int len = 5;
  aJsonObject *child = item->child;
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
      ret = printValue(child);
      entries[i++] = ret;
      if (ret)
        len += strlen(ret) + 2;
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
const char* aJsonClass::parseObject(aJsonObject *item, const char *value)
{
  aJsonObject *child;
  if (*value != '{')
    return NULL; // not an object!

  item->type = aJson_Object;
  value = skip(value + 1);
  if (*value == '}')
    return value + 1; // empty array.

  item->child = child = newItem();
  if (!item->child)
    return NULL;
  value = skip(parseString(child, skip(value)));
  if (!value)
    return NULL;
  child->name = child->value.valuestring;
  child->value.valuestring = NULL;
  if (*value != ':')
    return NULL; // fail!
  value = skip(parseValue(child, skip(value + 1))); // skip any spacing, get the value.
  if (!value)
    return NULL;

  while (*value == ',')
    {
      aJsonObject *new_item;
      if (!(new_item = newItem()))
        return NULL; // memory fail
      child->next = new_item;
      new_item->prev = child;
      child = new_item;
      value = skip(parseString(child, skip(value + 1)));
      if (!value)
        return NULL;
      child->name = child->value.valuestring;
      child->value.valuestring = NULL;
      if (*value != ':')
        return NULL; // fail!
      value = skip(parseValue(child, skip(value + 1))); // skip any spacing, get the value.
      if (!value)
        return NULL;
    }

  if (*value == '}')
    return value + 1; // end of array
  return NULL; // malformed.
}

// Render an object to text.
char* aJsonClass::printObject(aJsonObject *item)
{
  char **entries = 0, **names = 0;
  char *out = 0, *ptr, *ret, *str;
  int len = 7;
  unsigned char i = 0;
  aJsonObject *child = item->child;
  unsigned char numentries = 0, fail = 0;
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
  while (child)
    {
      names[i] = str = printStringPtr(child->name);
      entries[i++] = ret = printValue(child);
      if (str && ret)
        len += strlen(ret) + strlen(str) + 2;
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
  *ptr = 0;
  for (i = 0; i < numentries; i++)
    {
      strcpy(ptr, names[i]);
      ptr += strlen(names[i]);
      *ptr++ = ':';
      strcpy(ptr, entries[i]);
      ptr += strlen(entries[i]);
      if (i != numentries - 1)
        *ptr++ = ',';
      *ptr = 0;
      free(names[i]);
      free(entries[i]);
    }

  free(names);
  free(entries);
  *ptr++ = '}';
  *ptr++ = 0;
  return out;
}

// Get Array size/item / object item.
unsigned char aJsonClass::getArraySize(aJsonObject *array)
{
  aJsonObject *c = array->child;
  unsigned char i = 0;
  while (c)
    i++, c = c->next;
  return i;
}
aJsonObject* aJsonClass::getArrayItem(aJsonObject *array, unsigned char item)
{
  aJsonObject *c = array->child;
  while (c && item > 0)
    item--, c = c->next;
  return c;
}
aJsonObject* aJsonClass::getObjectItem(aJsonObject *object, const char *string)
{
  aJsonObject *c = object->child;
  while (c && strcmp(c->name, string))
    c = c->next;
  return c;
}

// Utility for array list handling.
void aJsonClass::suffixObject(aJsonObject *prev, aJsonObject *item)
{
  prev->next = item;
  item->prev = prev;
}
// Utility for handling references.
aJsonObject* aJsonClass::createReference(aJsonObject *item)
{
  aJsonObject *ref = newItem();
  if (!ref)
    return 0;
  memcpy(ref, item, sizeof(aJsonObject));
  ref->name = 0;
  ref->type |= aJson_IsReference;
  ref->next = ref->prev = 0;
  return ref;
}

// Add item to array/object.
void aJsonClass::addItemToArray(aJsonObject *array, aJsonObject *item)
{
  aJsonObject *c = array->child;
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
      suffixObject(c, item);
    }
}
void aJsonClass::addItemToObject(aJsonObject *object, const char *string, aJsonObject *item)
{
  if (!item)
    return;
  if (item->name)
    free(item->name);
  item->name = strdup(string);
  addItemToArray(object, item);
}
void aJsonClass::addItemReferenceToArray(aJsonObject *array, aJsonObject *item)
{
  addItemToArray(array, createReference(item));
}
void aJsonClass::addItemReferenceToObject(aJsonObject *object, const char *string, aJsonObject *item)
{
  addItemToObject(object, string, createReference(item));
}

aJsonObject* aJsonClass::detachItemFromArray(aJsonObject *array, unsigned char which)
{
  aJsonObject *c = array->child;
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
void aJsonClass::deleteItemFromArray(aJsonObject *array, unsigned char which)
{
  deleteItem(detachItemFromArray(array, which));
}
aJsonObject* aJsonClass::detachItemFromObject(aJsonObject *object, const char *string)
{
  unsigned char i = 0;
  aJsonObject *c = object->child;
  while (c && strcmp(c->name, string))
    i++, c = c->next;
  if (c)
    return detachItemFromArray(object, i);
  return 0;
}
void aJsonClass::deleteItemFromObject(aJsonObject *object, const char *string)
{
  deleteItem(detachItemFromObject(object, string));
}

// Replace array/object items with new ones.
void aJsonClass::replaceItemInArray(aJsonObject *array, unsigned char which, aJsonObject *newitem)
{
  aJsonObject *c = array->child;
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
  deleteItem(c);
}
void aJsonClass::replaceItemInObject(aJsonObject *object, const char *string, aJsonObject *newitem)
{
  unsigned char i = 0;
  aJsonObject *c = object->child;
  while (c && strcmp(c->name, string))
    i++, c = c->next;
  if (c)
    {
      newitem->name = strdup(string);
      replaceItemInArray(object, i, newitem);
    }
}

// Create basic types:
aJsonObject* aJsonClass::createNull()
{
  aJsonObject *item = newItem();
  if (item)
    item->type = aJson_NULL;
  return item;
}

aJsonObject* aJsonClass::createTrue()
{
  aJsonObject *item = newItem();
  if (item)
    {
      item->type = aJson_True;
      item->value.valuebool = -1;
    }
  return item;
}
aJsonObject* aJsonClass::createFalse()
{
  aJsonObject *item = newItem();
  if (item)
    {
      item->type = aJson_False;
      item->value.valuebool = 0;
    }
  return item;
}
aJsonObject* aJsonClass::createBool(char b)
{
  aJsonObject *item = newItem();
  if (item)
    {
      item->type = b ? aJson_True : aJson_False;
      item->value.valuebool = b ? -1 : 0;
    }
  return item;
}

aJsonObject* aJsonClass::createNumber(float num)
{
  aJsonObject *item = newItem();
  if (item)
    {
      item->type = aJson_Number;
      item->value.number.valuefloat = num;
      item->value.number.valueint = (int) num;
    }
  return item;
}

aJsonObject* aJsonClass::createString(const char *string)
{
  aJsonObject *item = newItem();
  if (item)
    {
      item->type = aJson_String;
      item->value.valuestring = strdup(string);
    }
  return item;
}

aJsonObject* aJsonClass::createArray()
{
  aJsonObject *item = newItem();
  if (item)
    item->type = aJson_Array;
  return item;
}
aJsonObject* aJsonClass::createObject()
{
  aJsonObject *item = newItem();
  if (item)
    item->type = aJson_Object;
  return item;
}

// Create Arrays:
aJsonObject* aJsonClass::createIntArray(int *numbers, unsigned char count)
{
  unsigned char i;
  aJsonObject *n = 0, *p = 0, *a = createArray();
  for (i = 0; a && i < count; i++)
    {
      n = createNumber(numbers[i]);
      if (!i)
        a->child = n;
      else
        suffixObject(p, n);
      p = n;
    }
  return a;
}

aJsonObject* aJsonClass::createFloatArray(float *numbers, unsigned char count)
{
  unsigned char i;
  aJsonObject *n = 0, *p = 0, *a = createArray();
  for (i = 0; a && i < count; i++)
    {
      n = createNumber(numbers[i]);
      if (!i)
        a->child = n;
      else
        suffixObject(p, n);
      p = n;
    }
  return a;
}

aJsonObject* aJsonClass::createDoubleArray(float *numbers, unsigned char count)
{
  unsigned char i;
  aJsonObject *n = 0, *p = 0, *a = createArray();
  for (i = 0; a && i < count; i++)
    {
      n = createNumber(numbers[i]);
      if (!i)
        a->child = n;
      else
        suffixObject(p, n);
      p = n;
    }
  return a;
}

aJsonObject* aJsonClass::createStringArray(const char **strings, unsigned char count)
{
  unsigned char i;
  aJsonObject *n = 0, *p = 0, *a = createArray();
  for (i = 0; a && i < count; i++)
    {
      n = createString(strings[i]);
      if (!i)
        a->child = n;
      else
        suffixObject(p, n);
      p = n;
    }
  return a;
}

void aJsonClass::addNullToObject(aJsonObject* object, const char* name) {
	addItemToObject(object, name, createNull());
}

void aJsonClass::addTrueToObject(aJsonObject* object,const char* name) {
	addItemToObject(object, name, createTrue());
}

void aJsonClass::addFalseToObject(aJsonObject* object,const char* name) {
	addItemToObject(object, name, createFalse());
}

void aJsonClass::addNumberToObject(aJsonObject* object,const char* name, int n) {
	addItemToObject(object, name, createNumber(n));
}

void aJsonClass::addStringToObject(aJsonObject* object,const char* name, const char* s) {
	addItemToObject(object, name, createString(s));
}

aJsonClass aJson;
