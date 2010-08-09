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
typedef struct aJSON_Object {
	struct aJSON_Object *next, *prev; // next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem
	struct aJSON_Object *child; // An array or object item will have a child pointer pointing to a chain of the items in the array/object.

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
} aJSON_Object;

class aJsonClass {
	/******************************************************************************
	 * Constructors
	 ******************************************************************************/

	/******************************************************************************
	 * User API
	 ******************************************************************************/
public:
	// Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished.
	aJSON_Object* parse(const char *value);
	// Render a aJSON_Object entity to text for transfer/storage. Free the char* when finished.
	char* print(aJSON_Object *item);
	// Render a aJSON_Object entity to text for transfer/storage without any formatting. Free the char* when finished.
	char* printUnformatted(aJSON_Object *item);
	// Delete a aJSON_Object entity and all sub-entities.
	void delete_Item(aJSON_Object *c);

	// Returns the number of items in an array (or object).
	unsigned char getArraySize(aJSON_Object *array);
	// Retrieve item number "item" from array "array". Returns NULL if unsuccessful.
	aJSON_Object* getArrayItem(aJSON_Object *array, unsigned char item);
	// Get item "string" from object. Case insensitive.
	aJSON_Object* getObjectItem(aJSON_Object *object, const char *string);

	// These calls create a aJSON_Object item of the appropriate type.
	aJSON_Object* createNull();
	aJSON_Object* createTrue();
	aJSON_Object* createFalse();
	aJSON_Object* createBool(char b);
	aJSON_Object* createNumber(float num);
	aJSON_Object* createString(const char *string);
	aJSON_Object* createArray();
	aJSON_Object* createObject();

	// These utilities create an Array of count items.
	aJSON_Object* createIntArray(int *numbers, unsigned char count);
	aJSON_Object* createFloatArray(float *numbers, unsigned char count);
	aJSON_Object* createDoubleArray(float *numbers, unsigned char count);
	aJSON_Object* createStringArray(const char **strings, unsigned char count);

	// Append item to the specified array/object.
	void addItemToArray(aJSON_Object *array, aJSON_Object *item);
	void addItemToObject(aJSON_Object *object, const char *string, aJSON_Object *item);
	// Append reference to item to the specified array/object. Use this when you want to add an existing aJSON_Object to a new aJSON_Object, but don't want to corrupt your existing aJSON_Object.
	void addItemReferenceToArray(aJSON_Object *array, aJSON_Object *item);
	void addItemReferenceToObject(aJSON_Object *object, const char *string,
			aJSON_Object *item);

	// Remove/Detach items from Arrays/Objects.
	aJSON_Object* detachItemFromArray(aJSON_Object *array, unsigned char which);
	void deleteItemFromArray(aJSON_Object *array, unsigned char which);
	aJSON_Object* detachItemFromObject(aJSON_Object *object, const char *string);
	void deleteItemFromObject(aJSON_Object *object, const char *string);

	// Update array items.
	void replaceItemInArray(aJSON_Object *array, unsigned char which, aJSON_Object *newitem);
	void replaceItemInObject(aJSON_Object *object, const char *string, aJSON_Object *newitem);

	void addNullToObject(aJSON_Object* object, const char* name);
	void addTrueToObject(aJSON_Object* object,const char* name);
	void addFalseToObject(aJSON_Object* object,const char* name);
	void addNumberToObject(aJSON_Object* object,const char* name, int n);
	void addStringToObject(aJSON_Object* object,const char* name, const char* s);

private:
	aJSON_Object* new_Item();
	const char* parse_number(aJSON_Object *item, const char *num);
	static char* print_number(aJSON_Object *item);
	const char* parse_string(aJSON_Object *item, const char *str);
	char* print_string_ptr(const char *str);
	char* print_string(aJSON_Object *item);
	const char* skip(const char *in);
	const char* parse_value(aJSON_Object *item, const char *value);
	char* print_value(aJSON_Object *item, unsigned char depth, char fmt);
	const char* parse_array(aJSON_Object *item, const char *value);
	const char* parse_object(aJSON_Object *item, const char *value);
	char* print_object(aJSON_Object *item, unsigned char depth, char fmt);
	void suffix_object(aJSON_Object *prev, aJSON_Object *item);
	aJSON_Object* create_reference(aJSON_Object *item);
	char* print_array(aJSON_Object *item, unsigned char depth, char fmt);

};

#endif
