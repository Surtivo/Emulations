#pragma once

#define LOAD_MEM 0b00000000
#define LOAD_ACC 0b00000001
#define STORE 0b00000010
#define ADD 0b00000011
#define SUB 0b00000100
#define MUL 0b00000101
#define DIV 0b00000110
#define INC 0b00000111
#define DEC 0b00001000
#define AND 0b00001001
#define OR 0b00001010
#define NOT 0b00001011
#define JMP 0b00001100
#define JZ 0b00001101
#define JNZ 0b00001110
#define JG 0b00001111
#define JL 0b00010000
#define JGE 0b00010001
#define JLE 0b00010010
#define HALT 0b00010011

#define MAX_SIZE 255
#define MIN_SIZE 0


typedef struct Instruction {
	unsigned char opcode;
	unsigned char operand;
} Instruction;
