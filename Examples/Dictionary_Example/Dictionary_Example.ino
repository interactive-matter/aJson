#include <aJSON.h>
#include <avr/pgmspace.h>

char *dictionary[] = {"id", "name", "age", NULL};

void setup() {
  Serial.begin(9600);
  freeMem("start");
}

void createObjectArray(bool useDictionary) {

  if (useDictionary){
    aJson.useDictionary(dictionary);
  } 
  else {
    aJson.useDictionary(NULL);
  }

  aJsonObject* users = aJson.createArray();
  
  for (int i = 0; i < 15; i++){
    aJsonObject * user=aJson.createObject();  
    aJson.addItemToObject(user, "name", aJson.createItem("Test"));
    aJson.addItemToObject(user, "id", aJson.createItem(i));
    aJson.addItemToObject(user, "age", aJson.createItem((int)random(1,100)));
    aJson.addItemToArray(users, user);
  }

  aJsonStream printStream(&Serial);
  aJson.print(users, &printStream);
  Serial.println();

  if (useDictionary){
    freeMem("Free memory with dictionary");
  }
  else {
    freeMem("Free memory without dictionary");
  }

  aJson.deleteItem(users);

}

void loop() {
  createObjectArray(false);
  createObjectArray(true);
  delay(5000);
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
    Serial.write(c);
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
