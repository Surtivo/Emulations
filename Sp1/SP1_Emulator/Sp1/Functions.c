#include <stdio.h>
#include <stdint.h>
#include "Functions.h"
#include "Definitions.h"

void statusFlags(unsigned char* status, int calculation, unsigned char acc, unsigned char memory) {

	*status = 0; //Clear all flags first;
	int8_t ac = (int8_t)acc;
	int8_t mem = (int8_t)memory;
	int8_t res = (int8_t)calculation;

	if (calculation == 0)
		*status |= 0b00000001; //Set Zero Flag;
	if (calculation > MAX_SIZE || calculation < MIN_SIZE)
		*status |= 0b00000010; //Set Carry Flag;
	if ((ac > 0 && mem > 0 && res < 0) || (ac < 0 && mem < 0 && res >= 0))
		*status |= 0b00000100; //Set Overflow Flag;
	// 	if (res < 0)
	// 		*status |= 0b00001000; //Set Negative Flag;
}