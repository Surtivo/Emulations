#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "Definitions.h"
#include "Functions.h"

void modo_raw(int ativar) {
    static struct termios oldt, newt;
    if (ativar) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); //Não trava o programa;
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

void print_tab(unsigned char tab[4][4]){
	for(int i=0; i<4; i++){
		for (int j=0; j<4; j++){
			printf("%d ", tab[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int main(int argc, char** argv) {
    Instruction program[MAX_SIZE];
	unsigned char memory[MAX_SIZE] = { 0 };
	unsigned char pc = 0;
	unsigned char status = 0;
	unsigned char opcode;
	unsigned char operand;
	unsigned char acc = 0;
	short int calculation;

	short int aux = 0;
	unsigned char tab[4][4] = {0};
	unsigned char pos_x = 0;
	unsigned char pos_y = 0;

	FILE* fptr = fopen(argv[1], "rb");
	//Se o arquivo nao for encontrato;
	if (fptr == NULL) {
		printf("Erro abrindo arquivo\n");
		fclose(fptr);
		return 1;
	}

	//Joga todo o conteudo do arquivo no array do programa;
	fread(&program, sizeof(Instruction) * 256, 1, fptr);
	fclose(fptr);

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	modo_raw(1);

	tab[pos_y][pos_x] = 1;
	print_tab(tab);
	tab[pos_y][pos_x] = 0;
	printf("%d\n", pos_y);

	while (1) {
		
		opcode = program[pc].opcode;
		operand = program[pc].operand;
		pc++;

		int ch = getchar(); 
        if (ch != EOF) {
            memory[255] = (unsigned char)ch;
			if(memory[254] >= 0){
				pos_y = memory[254];
				tab[pos_y][pos_x] = 1;
				print_tab(tab);
				tab[pos_y][pos_x] = 0;
			}			
			if(memory[253] >= 0){
				pos_x = memory[253];
				tab[pos_y][pos_x] = 1;
				print_tab(tab);
				tab[pos_y][pos_x] = 0;
			}
        }		

		switch (opcode) {
			case LOAD_MEM:
				acc = memory[operand];
				statusFlags(&status, acc, 0, 0);
				break;

			case LOAD_ACC:
				acc = operand;
				statusFlags(&status, acc, 0, 0);
				break;

			case STORE:
				memory[operand] = acc;
				break;

			case ADD:
				calculation = acc + memory[operand];
				statusFlags(&status, calculation, acc, memory[operand]);
				acc += memory[operand];
				break;

			case SUB:
				calculation = acc - memory[operand];
				statusFlags(&status, calculation, acc, memory[operand]);
				acc -= memory[operand];
				break;

			case MUL:
				calculation = acc * memory[operand];
				statusFlags(&status, calculation, acc, memory[operand]);
				acc *= memory[operand];
				break;

			case DIV:
				calculation = acc / memory[operand];
				statusFlags(&status, calculation, acc, memory[operand]);
				acc /= memory[operand];
				break;

			case INC:
				calculation = acc + 1;
				statusFlags(&status, calculation, acc, operand);
				acc++;
				break;

			case DEC:
				// printf("Decremento executado!\n");
				calculation = acc - 1;
				statusFlags(&status, calculation, acc, operand);
				acc--;
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
				printf("Acumulador = %d\n", acc);
				printf("Status = %d\n", status);
				printf("PC = %d\n", pc);
				printf("Program halted.\n");
				return 0;

			default:
				printf("Unknown opcode: %02X\n", opcode);
				return 1;
		}
		usleep(1000);
	}
	modo_raw(0);
    return 0;
}
