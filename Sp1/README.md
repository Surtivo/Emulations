# SP1 Assembly Emulator

<<<<<<< HEAD
This is a SP1 Processor Emulator for Load-Store Assembler. 8 bits for opcode and operand, making it a 16b its instruction.
=======
This is a SP1 Processor Emulator for Load-Store Assembler.

To this moment, some statuses were over simplified:
- Carry: Normal carry, occurs when unsigned char limit is exceeded;
- Overflow: It's a carry, but only for MUL and DIV;
>>>>>>> 48f3bc0b039a9541e9ca34c3ecb89c73cdc3cfb1

The statuses where corrected:
- Carry: Normal carry, occurs when unsigned char limit is exceeded;
- Overflow: Occurs when a addition or subtraciton is mathematically impossible to represent with the available bits;

## Instructions:

1 - Go to the folder "SP1Assembler" and compile the Assembler using the command:
      
      $make 

2 - Then mount the test file (test.asm):

      $./sp1a test.asm -o test.bin

3 - Go to the folder "SP1_Emulator/SP1" and compile the emulator:

      $gcc Emulator.c Functions.c -o emulator

4 - Use the emulator to run the program test.bin (remember to use the bin folder path or copy and paste him to the same directory):

      $./emulator test.bin
<<<<<<< HEAD

## Programs Used:
Project developed in Visual Studio (2022) and tested with GCC on a Lubuntu Virtual Machine.
=======
>>>>>>> 48f3bc0b039a9541e9ca34c3ecb89c73cdc3cfb1
