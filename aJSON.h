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
typedef struct aJsonObject {
	struct aJsonObject *next, *prev; // next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem
	struct aJsonObject *child; // An array or object item will have a child pointer pointing to a chain of the items in the array/object.

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
	aJsonObject* parse(const char *value);
	// Render a aJsonObject entity to text for transfer/storage. Free the char* when finished.
	char* print(aJsonObject *item);
	// Render a aJsonObject entity to text for transfer/storage without any formatting. Free the char* when finished.
	char* printUnformatted(aJsonObject *item);
	// Delete a aJsonObject entity and all sub-entities.
	void deleteItem(aJsonObject *c);

	// Returns the number of items in an array (or object).
	unsigned char getArraySize(aJsonObject *array);
	// Retrieve item number "item" from array "array". Returns NULL if unsuccessful.
	aJsonObject* getArrayItem(aJsonObject *array, unsigned char item);
	// Get item "string" from object. Case insensitive.
	aJsonObject* getObjectItem(aJsonObject *object, const char *string);

	// These calls create a aJsonObject item of the appropriate type.
	aJsonObject* createNull();
	aJsonObject* createTrue();
	aJsonObject* createFalse();
	aJsonObject* createBool(char b);
	aJsonObject* createNumber(float num);
	aJsonObject* createString(const char *string);
	aJsonObject* createArray();
	aJsonObject* createObject();

	// These utilities create an Array of count items.
	aJsonObject* createIntArray(int *numbers, unsigned char count);
	aJsonObject* createFloatArray(float *numbers, unsigned char count);
	aJsonObject* createDoubleArray(float *numbers, unsigned char count);
	aJsonObject* createStringArray(const char **strings, unsigned char count);

	// Append item to the specified array/object.
	void addItemToArray(aJsonObject *array, aJsonObject *item);
	void addItemToObject(aJsonObject *object, const char *string, aJsonObject *item);
	// Append reference to item to the specified array/object. Use this when you want to add an existing aJsonObject to a new aJsonObject, but don't want to corrupt your existing aJsonObject.
	void addItemReferenceToArray(aJsonObject *array, aJsonObject *item);
	void addItemReferenceToObject(aJsonObject *object, const char *string,
			aJsonObject *item);

	// Remove/Detach items from Arrays/Objects.
	aJsonObject* detachItemFromArray(aJsonObject *array, unsigned char which);
	void deleteItemFromArray(aJsonObject *array, unsigned char which);
	aJsonObject* detachItemFromObject(aJsonObject *object, const char *string);
	void deleteItemFromObject(aJsonObject *object, const char *string);

	// Update array items.
	void replaceItemInArray(aJsonObject *array, unsigned char which, aJsonObject *newitem);
	void replaceItemInObject(aJsonObject *object, const char *string, aJsonObject *newitem);

	void addNullToObject(aJsonObject* object, const char* name);
	void addTrueToObject(aJsonObject* object,const char* name);
	void addFalseToObject(aJsonObject* object,const char* name);
	void addNumberToObject(aJsonObject* object,const char* name, int n);
	void addStringToObject(aJsonObject* object,const char* name, const char* s);

private:
	aJsonObject* newItem();
	const char* parseNumber(aJsonObject *item, const char *num);
	char* printNumber(aJsonObject *item);

	const char* parseString(aJsonObject *item, const char *str);
	char* printStringPtr(const char *str);
	char* printString(aJsonObject *item);

	const char* skip(const char *in);

	const char* parseValue(aJsonObject *item, const char *value);
	char* printValue(aJsonObject *item);

	const char* parseArray(aJsonObject *item, const char *value);
	char* printArray(aJsonObject *item);

	const char* parseObject(aJsonObject *item, const char *value);
	char* printObject(aJsonObject *item);
	void suffixObject(aJsonObject *prev, aJsonObject *item);

	aJsonObject* createReference(aJsonObject *item);

};

extern aJsonClass aJson;

#endif
