#include <WProgram.h>
#include "aJSON.h"
#include <avr/pgmspace.h>

const prog_char PROGMEM RETRIEVING_NAME[] ="Retrieving name\n";
const prog_char PROGMEM ERROR_RETRIEVING_NAME[] ="Error retrieving name\n";
const prog_char PROGMEM SUCCESSFULLY_RETRIEVED_NAME[] ="Successfully retrieved Name:";
const prog_char PROGMEM PARSING_OBJECT[] ="Parsing String\n";
const prog_char PROGMEM ERROR_PARSING_OBJECT[] ="Error parsing Object\n";
const prog_char PROGMEM SUCCESSFULLY_PARSED_OBJECT[] ="Successfully parsed Object\n";
const prog_char PROGMEM DELETING_OBJECT_STRING[] = "Deleting the object\n";
const prog_char PROGMEM FORMAT_FAILED_STRING[] = "Failed to create Format Object\n";
const prog_char PROGMEM OUTPUT_STRING_ERROR[] = "Error creating output String\n";
const prog_char PROGMEM RESULT_PRINTING_STRING[] = "Printing the result:\n";
const prog_char PROGMEM ADDING_FRAMERATE_STRING[] = "Adding frame rate to the format\n";
const prog_char PROGMEM ADDING_INTERLACE_STRING[] = "Adding interlace to the format\n";
const prog_char PROGMEM ADDING_HEIGHT_STRING[] = "Adding height to the format\n";
const prog_char PROGMEM ADDING_WIDTH_STRING[] = "Adding width to the format\n";
const prog_char PROGMEM ADDING_TYPE_STRING[] = "Adding type to the format\n";
const prog_char PROGMEM ADDING_FORMAT_STRING[] = "Adding format to the object\n";
const prog_char PROGMEM CREATING_FROMAT_STRING[] = "Creating format object\n";
const prog_char PROGMEM ADDING_NAME_STRING[] = "Adding name to the object\n";
const prog_char PROGMEM OBJECT_CREATION_FAILED_STRING[] = "Failed to create the object\n";
const prog_char PROGMEM OBJECT_CREATE_STRING[] = "Created a Object\n";
const prog_char PROGMEM HELLO_STRING[] = "Testing aJson\n";

//somehow this is needed to use Serial
extern "C" void __cxa_pure_virtual() {
	while (1)
		;
}

// given a PROGMEM string, use Serial.print() to send it out
// this is needed to save precious memory
//htanks to todbot for this http://todbot.com/blog/category/programming/
void printProgStr(const prog_char* str) {
	char c;
	if (!str) {
		return;
	}
	while ((c = pgm_read_byte(str))) {
		Serial.print(c, BYTE);
		str++;
	}
}

// The setup() method runs once, when the sketch starts
void setup() {
	Serial.begin(9600);
	printProgStr( HELLO_STRING);
	aJson_Object* root = aJson.createObject();
	if (root != NULL) {
		printProgStr( OBJECT_CREATE_STRING);
	} else {
		printProgStr( OBJECT_CREATION_FAILED_STRING);
		return;
	}
	printProgStr( ADDING_NAME_STRING);
	aJson.addItemToObject(root, "name", aJson.createString(
			"Jack (\"Bee\") Nimble"));
/*	printProgStr( CREATING_FROMAT_STRING);
	aJson_Object* fmt = aJson.createObject();
	if (fmt != NULL) {
		printProgStr( ADDING_FORMAT_STRING);
		aJson.addItemToObject(root, "format", fmt);
		printProgStr( ADDING_TYPE_STRING);
		aJson.addStringToObject(fmt, "type", "rect");
		printProgStr( ADDING_WIDTH_STRING);
		aJson.addNumberToObject(fmt, "width", 1920);
		printProgStr( ADDING_HEIGHT_STRING);
		aJson.addNumberToObject(fmt, "height", 1080);
		printProgStr( ADDING_INTERLACE_STRING);
		aJson.addFalseToObject(fmt, "interlace");
		printProgStr( ADDING_FRAMERATE_STRING);
		aJson.addNumberToObject(fmt, "frame rate", 24);
	} else {
		printProgStr( FORMAT_FAILED_STRING);
		return;
	}
*/

	printProgStr( RESULT_PRINTING_STRING);
	char* string = aJson.print(root);
	if (string != NULL) {
		Serial.println(string);
	} else {
		printProgStr( OUTPUT_STRING_ERROR);
	}

	printProgStr( DELETING_OBJECT_STRING);
	aJson.deleteItem(root);

	printProgStr( PARSING_OBJECT);
	root = aJson.parse(string);
	free(string);
	if (root != NULL) {
		printProgStr( SUCCESSFULLY_PARSED_OBJECT);
	} else {
		printProgStr( ERROR_PARSING_OBJECT);
		return;
	}
	aJson_Object* name = aJson.getObjectItem(root, "name");
	if (name != NULL) {
		printProgStr( SUCCESSFULLY_RETRIEVED_NAME);
		Serial.println(name->value.valuestring);
	} else {
		printProgStr( ERROR_RETRIEVING_NAME);
		return;
	}

}

// the loop() method runs over and over again,
// as long as the Arduino has power
void loop() {
}

int main(void) {
	init();

	setup();

	for (;;)
		loop();

	return 0;
}

