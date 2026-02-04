# SP1-Assembly-Emulator

This is a SP1 Processor Emulator for Load-Store Assembler.
    To this moment, some statuses were over simplified:
        Carry: Normal carry, occurs when unsigned char limit is exceeded;
        Overflow: It's a carry, but only for MUL and DIV;

# Instructions:

1 - Go to the folder "SP1Assembler" and compile the Assembler using the command:
      
      $make 

2 - Then mount the test file (test.asm) 

      $./sp1a test.asm -o test.bin

3 - Go to the folder "SP1_Emulator/SP1" and compile the emulator

      $gcc Emulator.c Functions.c -o emulator

4 - Use the emulator to run the program test.bin

      $./emulator test.bin