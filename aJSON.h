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

#ifndef aJson__h
#define aJson__h

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

#define cJSON_IsReference 128

// The cJSON structure:
typedef struct aJson_Object {
	struct aJson_Object *next, *prev; // next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem
	struct aJson_Object *child; // An array or object item will have a child pointer pointing to a chain of the items in the array/object.

	char type; // The type of the item, as above.

	union {
		char *valuestring; // The item's string, if type==cJSON_String
		char valuebool; //the items value for true & false
		struct {
			int valueint; // The item's number, if type==cJSON_Number
			float valuefloat; // The item's number, if type==cJSON_Number
		} number;
	} value;

	char *string; // The item's name string, if this item is the child of, or is in the list of subitems of an object.
} aJson_Object;

class aJsonClass {
	/******************************************************************************
	 * Constructors
	 ******************************************************************************/

	/******************************************************************************
	 * User API
	 ******************************************************************************/
public:
	// Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished.
	aJson_Object* parse(const char *value);
	// Render a aJson_Object entity to text for transfer/storage. Free the char* when finished.
	char* print(aJson_Object *item);
	// Render a aJson_Object entity to text for transfer/storage without any formatting. Free the char* when finished.
	char* printUnformatted(aJson_Object *item);
	// Delete a aJson_Object entity and all sub-entities.
	void deleteItem(aJson_Object *c);

	// Returns the number of items in an array (or object).
	unsigned char getArraySize(aJson_Object *array);
	// Retrieve item number "item" from array "array". Returns NULL if unsuccessful.
	aJson_Object* getArrayItem(aJson_Object *array, unsigned char item);
	// Get item "string" from object. Case insensitive.
	aJson_Object* getObjectItem(aJson_Object *object, const char *string);

	// These calls create a aJson_Object item of the appropriate type.
	aJson_Object* createNull();
	aJson_Object* createTrue();
	aJson_Object* createFalse();
	aJson_Object* createBool(char b);
	aJson_Object* createNumber(float num);
	aJson_Object* createString(const char *string);
	aJson_Object* createArray();
	aJson_Object* createObject();

	// These utilities create an Array of count items.
	aJson_Object* createIntArray(int *numbers, unsigned char count);
	aJson_Object* createFloatArray(float *numbers, unsigned char count);
	aJson_Object* createDoubleArray(float *numbers, unsigned char count);
	aJson_Object* createStringArray(const char **strings, unsigned char count);

	// Append item to the specified array/object.
	void addItemToArray(aJson_Object *array, aJson_Object *item);
	void addItemToObject(aJson_Object *object, const char *string, aJson_Object *item);
	// Append reference to item to the specified array/object. Use this when you want to add an existing aJson_Object to a new aJson_Object, but don't want to corrupt your existing aJson_Object.
	void addItemReferenceToArray(aJson_Object *array, aJson_Object *item);
	void addItemReferenceToObject(aJson_Object *object, const char *string,
			aJson_Object *item);

	// Remove/Detach items from Arrays/Objects.
	aJson_Object* detachItemFromArray(aJson_Object *array, unsigned char which);
	void deleteItemFromArray(aJson_Object *array, unsigned char which);
	aJson_Object* detachItemFromObject(aJson_Object *object, const char *string);
	void deleteItemFromObject(aJson_Object *object, const char *string);

	// Update array items.
	void replaceItemInArray(aJson_Object *array, unsigned char which, aJson_Object *newitem);
	void replaceItemInObject(aJson_Object *object, const char *string, aJson_Object *newitem);

	void addNullToObject(aJson_Object* object, const char* name);
	void addTrueToObject(aJson_Object* object,const char* name);
	void addFalseToObject(aJson_Object* object,const char* name);
	void addNumberToObject(aJson_Object* object,const char* name, int n);
	void addStringToObject(aJson_Object* object,const char* name, const char* s);

private:
	aJson_Object* newItem();
	const char* parseNumber(aJson_Object *item, const char *num);
	char* printNumber(aJson_Object *item);

	const char* parseString(aJson_Object *item, const char *str);
	char* printStringPtr(const char *str);
	char* printString(aJson_Object *item);

	const char* skip(const char *in);

	const char* parseValue(aJson_Object *item, const char *value);
	char* printValue(aJson_Object *item);

	const char* parseArray(aJson_Object *item, const char *value);
	char* printArray(aJson_Object *item);

	const char* parseObject(aJson_Object *item, const char *value);
	char* printObject(aJson_Object *item);
	void suffixObject(aJson_Object *prev, aJson_Object *item);

	aJson_Object* createReference(aJson_Object *item);

};

extern aJsonClass aJson;

#endif
