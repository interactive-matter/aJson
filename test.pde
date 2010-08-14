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
        aJsonObject* root = aJson.createObject();
        if (root != NULL) {
                printProgStr( OBJECT_CREATE_STRING);
        } else {
                printProgStr( OBJECT_CREATION_FAILED_STRING);
                return;
        }
        printProgStr( ADDING_NAME_STRING);
        aJson.addItemToObject(root, "name", aJson.createString(
                        "Jack (\"Bee\") Nimble"));
/*      printProgStr( CREATING_FROMAT_STRING);
        aJsonObject* fmt = aJson.createObject();
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
        aJsonObject* name = aJson.getObjectItem(root, "name");
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