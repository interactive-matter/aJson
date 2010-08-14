#include <WProgram.h>
#include "aJSON.h"

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

