#include "Functions.h"
#include <stdio.h>

#define MAX_SIZE 255
#define MIN_SIZE 0

void statusFlags(unsigned char* status, int calculation){
	if (calculation == 0)
		*status = 0b00000001; //Set Zero Flag;
	else if (calculation > MAX_SIZE || calculation < MIN_SIZE)
		*status = 0b00000010; //Set Carry Flag;
	else if (((calculation >> 8 & 1) == 1))
		*status = 0b00000011; //Set Overflow Flag;
	else
		*status = 0b00000000; //Clear Flag;
}