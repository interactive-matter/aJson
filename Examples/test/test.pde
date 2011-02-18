#include <aJSON.h>


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
const prog_char PROGMEM ADDING_LENGTH_STRING[] = "Adding length to the object\n";
const prog_char PROGMEM CREATING_FROMAT_STRING[] = "Creating format object\n";
const prog_char PROGMEM ADDING_NAME_STRING[] = "Adding name to the object\n";
const prog_char PROGMEM OBJECT_CREATION_FAILED_STRING[] = "Failed to create the object\n";
const prog_char PROGMEM OBJECT_CREATE_STRING[] = "Created a Object\n";
const prog_char PROGMEM HELLO_STRING[] = "********************\nTesting aJson\n*****************\n";

// The setup() method runs once, when the sketch starts
void setup() {
  Serial.begin(9600);
  printProgStr(HELLO_STRING);
  freeMem("start");
}

void testObjects() {

  aJsonObject* root = aJson.createObject();
  if (root != NULL) {
    printProgStr( OBJECT_CREATE_STRING);
  } 
  else {
    printProgStr( OBJECT_CREATION_FAILED_STRING);
    return;
  }
  printProgStr( ADDING_NAME_STRING);
  aJson.addItemToObject(root, "name", aJson.createItem(
  "Jack (\"Bee\") Nimble"));
  printProgStr( CREATING_FROMAT_STRING);
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
    printProgStr( ADDING_LENGTH_STRING);
    aJson.addNumberToObject(fmt, "length", 1.29);
  } 
  else {
    printProgStr( FORMAT_FAILED_STRING);
    return;
  }

  freeMem("with object");
  printProgStr( RESULT_PRINTING_STRING);
  char* string = aJson.print(root);
  if (string != NULL) {
    Serial.println(string);
  } 
  else {
    printProgStr( OUTPUT_STRING_ERROR);
  }

  printProgStr( DELETING_OBJECT_STRING);
  aJson.deleteItem(root);
  freeMem("after deletion");

  printProgStr(PARSING_OBJECT);
  Serial.println(string);
  root = aJson.parse(string);
  free(string);
  freeMem("after printing");
  if (root != NULL) {
    printProgStr(SUCCESSFULLY_PARSED_OBJECT);
  } 
  else {
    printProgStr( ERROR_PARSING_OBJECT);
    return;
  }
  printProgStr( RESULT_PRINTING_STRING);
  string = aJson.print(root);
  if (string != NULL) {
    Serial.println(string);
    free(string);
  } 
  else {
    printProgStr( OUTPUT_STRING_ERROR);
  }
  freeMem("after 2nd printing");

  aJsonObject* name = aJson.getObjectItem(root, "name");
  if (name != NULL) {
    printProgStr( SUCCESSFULLY_RETRIEVED_NAME);
    Serial.println(name->valuestring);
  } 
  else {
    printProgStr( ERROR_RETRIEVING_NAME);
  }

  printProgStr( DELETING_OBJECT_STRING);
  aJson.deleteItem(root);
  freeMem("after deleting object");
}

void testArrays() {
  aJsonObject* root = aJson.createArray();
  if (root != NULL) {
    printProgStr( OBJECT_CREATE_STRING);
  } 
  else {
    printProgStr( OBJECT_CREATION_FAILED_STRING);
    return;
  }  
  aJsonObject* day;
  day=aJson.createItem("Monday");
  aJson.addItemToArray(root, day);
  day=aJson.createItem("Tuesday");
  aJson.addItemToArray(root, day);
  day=aJson.createItem("Wednesday");
  aJson.addItemToArray(root, day);
  day=aJson.createItem("Thursday");
  aJson.addItemToArray(root, day);
  day=aJson.createItem("Friday");
  aJson.addItemToArray(root, day);
  day=aJson.createItem("Saturday");
  aJson.addItemToArray(root, day);
  day=aJson.createItem("Sunday");
  aJson.addItemToArray(root, day);

  freeMem("with object");
  printProgStr( RESULT_PRINTING_STRING);

  char* string = aJson.print(root);
  if (string != NULL) {
    Serial.println(string);
  } 
  else {
    printProgStr( OUTPUT_STRING_ERROR);
  }

  printProgStr( DELETING_OBJECT_STRING);
  freeMem("after printing");

  aJson.deleteItem(root);
  freeMem("after deletion");  
  
  printProgStr(PARSING_OBJECT);
  Serial.println(string);
  root = aJson.parse(string);

  free(string);
  freeMem("after parsing");

  if (root != NULL) {
    printProgStr(SUCCESSFULLY_PARSED_OBJECT);
  } 
  else {
    printProgStr( ERROR_PARSING_OBJECT);
    return;
  }
  printProgStr( RESULT_PRINTING_STRING);

  string = aJson.print(root);
  if (string != NULL) {
    Serial.println(string);
    free(string);
  } 
  else {
    printProgStr( OUTPUT_STRING_ERROR);
  }

  aJson.deleteItem(root);
  freeMem("after 2nd printing");  
}


// the loop() method runs over and over again,
// as long as the Arduino has power
void loop() {
  testObjects();
  delay(1000);
  testArrays();
  delay(1000);
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

//Code to print out the free memory

struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

extern char * const __brkval;
extern struct __freelist *__flp;

uint16_t freeMem(uint16_t *biggest)
{
  char *brkval;
  char *cp;
  unsigned freeSpace;
  struct __freelist *fp1, *fp2;

  brkval = __brkval;
  if (brkval == 0) {
    brkval = __malloc_heap_start;
  }
  cp = __malloc_heap_end;
  if (cp == 0) {
    cp = ((char *)AVR_STACK_POINTER_REG) - __malloc_margin;
  }
  if (cp <= brkval) return 0;

  freeSpace = cp - brkval;

  for (*biggest = 0, fp1 = __flp, fp2 = 0;
     fp1;
     fp2 = fp1, fp1 = fp1->nx) {
      if (fp1->sz > *biggest) *biggest = fp1->sz;
    freeSpace += fp1->sz;
  }

  return freeSpace;
}

uint16_t biggest;

void freeMem(char* message) {
  Serial.print(message);
  Serial.print(":\t");
  Serial.println(freeMem(&biggest));
}



