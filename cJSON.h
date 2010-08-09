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

#ifndef cJSON__h
#define cJSON__h

/******************************************************************************
 * Definitions
 ******************************************************************************/
// cJSON Types:
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Number 3
#define cJSON_String 4
#define cJSON_Array 5
#define cJSON_Object 6

#define cJSON_IsReference 256

// The cJSON structure:
typedef struct cJSON {
	struct cJSON *next, *prev; // next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem
	struct cJSON *child; // An array or object item will have a child pointer pointing to a chain of the items in the array/object.

	int type; // The type of the item, as above.

	union {
		char *valuestring; // The item's string, if type==cJSON_String
		char valuebool; //the items value for true & false
		struct {
			int valueint; // The item's number, if type==cJSON_Number
			float valuedouble; // The item's number, if type==cJSON_Number
		} number;
	} value;

	char *string; // The item's name string, if this item is the child of, or is in the list of subitems of an object.
} cJSON;

class aJsonClass {
	/******************************************************************************
	 * Constructors
	 ******************************************************************************/

	/******************************************************************************
	 * User API
	 ******************************************************************************/
public:
	// Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished.
	cJSON* parse(const char *value);
	// Render a cJSON entity to text for transfer/storage. Free the char* when finished.
	char* print(cJSON *item);
	// Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished.
	char* printUnformatted(cJSON *item);
	// Delete a cJSON entity and all sub-entities.
	void delete_Item(cJSON *c);

	// Returns the number of items in an array (or object).
	unsigned char getArraySize(cJSON *array);
	// Retrieve item number "item" from array "array". Returns NULL if unsuccessful.
	cJSON* getArrayItem(cJSON *array, unsigned char item);
	// Get item "string" from object. Case insensitive.
	cJSON* getObjectItem(cJSON *object, const char *string);

	// These calls create a cJSON item of the appropriate type.
	cJSON* createNull();
	cJSON* createTrue();
	cJSON* createFalse();
	cJSON* createBool(char b);
	cJSON* createNumber(float num);
	cJSON* createString(const char *string);
	cJSON* createArray();
	cJSON* createObject();

	// These utilities create an Array of count items.
	cJSON* createIntArray(int *numbers, unsigned char count);
	cJSON* createFloatArray(float *numbers, unsigned char count);
	cJSON* createDoubleArray(float *numbers, unsigned char count);
	cJSON* createStringArray(const char **strings, unsigned char count);

	// Append item to the specified array/object.
	void addItemToArray(cJSON *array, cJSON *item);
	void addItemToObject(cJSON *object, const char *string, cJSON *item);
	// Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON.
	void addItemReferenceToArray(cJSON *array, cJSON *item);
	void addItemReferenceToObject(cJSON *object, const char *string,
			cJSON *item);

	// Remove/Detach items from Arrays/Objects.
	cJSON* detachItemFromArray(cJSON *array, unsigned char which);
	void deleteItemFromArray(cJSON *array, unsigned char which);
	cJSON* detachItemFromObject(cJSON *object, const char *string);
	void deleteItemFromObject(cJSON *object, const char *string);

	// Update array items.
	void replaceItemInArray(cJSON *array, unsigned char which, cJSON *newitem);
	void replaceItemInObject(cJSON *object, const char *string, cJSON *newitem);
private:
	cJSON* new_Item();
	const char* parse_number(cJSON *item, const char *num);
	static char* print_number(cJSON *item);
	const char* parse_string(cJSON *item, const char *str);
	char* print_string_ptr(const char *str);
	char* print_string(cJSON *item);
	const char* skip(const char *in);
	const char* parse_value(cJSON *item, const char *value);
	char* print_value(cJSON *item, unsigned char depth, char fmt);
	const char* parse_array(cJSON *item, const char *value);
	const char* parse_object(cJSON *item, const char *value);
	char* print_object(cJSON *item, unsigned char depth, char fmt);
	void suffix_object(cJSON *prev, cJSON *item);
	cJSON* create_reference(cJSON *item);
	char* print_array(cJSON *item, unsigned char depth, char fmt);

};

#define cJSON_AddNullToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddNumberToObject(object,name,n)	cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

#endif
