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

#include "test.pde"

int main(void) {
	init();

	setup();

	for (;;)
		loop();

	return 0;
}

