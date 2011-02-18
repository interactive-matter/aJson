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
 * Includes
 ******************************************************************************/
#include <stdio.h>


/******************************************************************************
 * Definitions
 ******************************************************************************/
// aJson Types:
#define aJson_False 0
#define aJson_True 1
#define aJson_NULL 2
#define aJson_Int 3
#define aJson_Float 4
#define aJson_String 5
#define aJson_Array 6
#define aJson_Object 7

#define aJson_IsReference 128

// The aJson structure:
typedef struct aJsonObject {
        char *name; // The item's name string, if this item is the child of, or is in the list of subitems of an object.
	struct aJsonObject *next, *prev; // next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem
	struct aJsonObject *child; // An array or object item will have a child pointer pointing to a chain of the items in the array/object.

	char type; // The type of the item, as above.

	union {
		char *valuestring; // The item's string, if type==aJson_String
		char valuebool; //the items value for true & false
		int valueint; // The item's number, if type==aJson_Number
		double valuefloat; // The item's number, if type==aJson_Number
	};
} aJsonObject;

class aJsonClass {
	/******************************************************************************
	 * Constructors
	 ******************************************************************************/

	/******************************************************************************
	 * User API
	 ******************************************************************************/
public:
	// Supply a block of JSON, and this returns a aJson object you can interrogate. Call aJson.deleteItem when finished.
        aJsonObject* parse(FILE* stream); //Reads from a stream
        aJsonObject* parse(FILE* stream,char** filter_values); //Read from a file, but only return values include in the char* array filter_values
	aJsonObject* parse(char *value); //Reads from a string
	// Render a aJsonObject entity to text for transfer/storage. Free the char* when finished.
	int print(aJsonObject *item, FILE* stream);
	char* print(aJsonObject* item);
	//Renders a aJsonObject directly to a output stream
	char stream(aJsonObject *item, FILE* stream);
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
	aJsonObject* createItem(char b);
	aJsonObject* createItem(int num);
	aJsonObject* createItem(double num);
	aJsonObject* createItem(const char *string);
	aJsonObject* createArray();
	aJsonObject* createObject();

	// These utilities create an Array of count items.
	aJsonObject* createIntArray(int *numbers, unsigned char count);
	aJsonObject* createFloatArray(double *numbers, unsigned char count);
	aJsonObject* createDoubleArray(double *numbers, unsigned char count);
	aJsonObject* createStringArray(const char **strings, unsigned char count);

	// Append item to the specified array/object.
	void addItemToArray(aJsonObject *array, aJsonObject *item);
	void addItemToObject(aJsonObject *object, const char *string,
			aJsonObject *item);
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
	void replaceItemInArray(aJsonObject *array, unsigned char which,
			aJsonObject *newitem);
	void replaceItemInObject(aJsonObject *object, const char *string,
			aJsonObject *newitem);

	void addNullToObject(aJsonObject* object, const char* name);
	void addTrueToObject(aJsonObject* object, const char* name);
	void addFalseToObject(aJsonObject* object, const char* name);
	void addNumberToObject(aJsonObject* object, const char* name, int n);
        void addNumberToObject(aJsonObject* object, const char* name, double n);
	void addStringToObject(aJsonObject* object, const char* name,
					const char* s);

private:
	aJsonObject* newItem();
	int parseNumber(aJsonObject *item, FILE* stream);
	int printInt(aJsonObject *item, FILE* stream);
	int printFloat(aJsonObject *item, FILE* stream);

	int parseString(aJsonObject *item, FILE* stream);
	int printStringPtr(const char *str, FILE* stream);
	int printString(aJsonObject *item, FILE* stream);

	int skip(FILE* stream);

	int parseValue(aJsonObject *item, FILE* stream, char** filter);
	int printValue(aJsonObject *item, FILE* stream);

	int parseArray(aJsonObject *item, FILE* stream, char** filter);
	int printArray(aJsonObject *item, FILE* stream);

	int parseObject(aJsonObject *item, FILE* stream, char** filter);
	int printObject(aJsonObject *item, FILE* stream);
	void suffixObject(aJsonObject *prev, aJsonObject *item);

	aJsonObject* createReference(aJsonObject *item);

};

extern aJsonClass aJson;

#endif
