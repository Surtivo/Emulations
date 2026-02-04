#include <stdio.h>
#include "Definitions.h"
#include "Functions.c"

int main() {
    Instruction program[MAX_SIZE] = { 0 };
	unsigned char memory[MAX_SIZE] = { 0 };
	unsigned char pc = 0;
	unsigned char status = 0;
	unsigned char opcode;
	unsigned char operand;
	unsigned char acc = 0;
	int calculation;

	while (true) {
		
		opcode = program[pc].opcode;
		operand = program[pc].operand;
		pc++;

		switch (opcode) {
			case LOAD_MEM:
				acc = memory[operand];
				statusFlags(&status, acc);
				break;

			case LOAD_ACC:
				acc = operand;
				statusFlags(&status, acc);
				break;

			case STORE:
				memory[operand] = acc;
				break;

			case ADD:
				calculation = acc + memory[operand];
				statusFlags(&status, calculation);
				acc += memory[operand];
				break;

			case SUB:
				calculation = acc - memory[operand];
				statusFlags(&status, calculation);
				acc -= memory[operand];
				break;

			case MUL:
				calculation = acc * memory[operand];
				statusFlags(&status, calculation);
				acc *= memory[operand];
				break;

			case DIV:
				calculation = acc / memory[operand];
				statusFlags(&status, calculation);
				acc /= memory[operand];
				break;

			case INC:
				calculation = acc + 1;
				statusFlags(&status, calculation);
				acc++;
				break;

			case DEC:
				calculation = acc - 1;
				statusFlags(&status, calculation);
				acc--;
				//printf("%d, %d, %d\n", acc, calculation, status);
				break;

			case AND:
				acc &= memory[operand];
				break;

			case OR:
				acc |= memory[operand];
				break;

			case NOT:
				acc = ~acc;
				break;

			case JMP:
				pc = operand / 2;
				break;

			case JZ:
				if (acc == 0) {
					pc = operand / 2;
				}
				break;

			case JNZ:
				if (acc != 0) {
					pc = operand / 2;
				}
				break;

			case JG:
				if (acc > 0) {
					pc = operand / 2;
				}
				break;

			case JL:
				if (acc < 0) {
					pc = operand / 2;
				}
				break;

			case JGE:
				if (acc >= 0) {
					pc = operand / 2;
				}
				break;

			case JLE:
				if (acc <= 0) {
					pc = operand / 2;
				}
				break;

			case HALT:
				printf("Program halted.\n");
				return 0;

			default:
				printf("Unknown opcode: %02X\n", opcode);
				return 1;
		}
	}

    return 0;
}
