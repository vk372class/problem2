#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lc3.h"

// you can define a simple memory module here for this program
Register memory[32];   // 32 words of memory enough to store simple program

int controller (CPU_p cpu) {
    // check to make sure both pointers are not NULL
    // do any initializations here
    ALU_p alu = malloc(sizeof(struct ALU_s));
    Register opcode, Rd, Rs1, Rs2, immed_offset;    // fields for the IR
    int state = FETCH, n, z, p;
    for (;;) {   // efficient endless loop
        switch (state) {
            case FETCH: // microstates 18, 33, 35 in the book
                printf("Here in FETCH\n");
                // get memory[PC] into IR - memory is a global array
                cpu->MAR = cpu->PC;
                cpu->PC++;                 // increment PC
                cpu->MDR = memory[cpu->MAR];
                cpu->IR = cpu->MDR;

                //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                printf("MAR: %hu\n",cpu->MAR);
                printf("PC: %hu\n",cpu->PC);
                printf("MDR: %hu\n",cpu->MDR);
                printf("Memory[0]: %hu\n",memory[0]);
                printf("IR: %hu\n",cpu->IR);
                 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
               state = DECODE;
                break;
            case DECODE: // microstate 32
                // get the fields out of the IR
                opcode = cpu->IR & 0xF000;
                opcode = opcode >> 12;
                Rd = cpu->IR & 0x0E00;
                Rd = Rd >> 9;
                Rs1 = cpu->IR & 0x01C0;
                Rs1 = Rs1 >> 6;
                Rs2 = cpu->IR & 0x0007;

                printf("\nopcode: %d \nrd: %d \nrs1: %d \nrs2: %d", opcode, Rd, Rs1, Rs2);

                // make sure opcode is in integer form
                
                // hint: use four unsigned int variables, opcode, Rd, Rs, and immed7
                // extract the bit fields from the IR into these variables
                state = EVAL_ADDR;
                break;
            case EVAL_ADDR: // Look at the LD instruction to see microstate 2 example
                switch (opcode) {
                    case ADD:
                            break;
                       
                     
                // different opcodes require different handling
                // compute effective address, e.g. add sext(immed7) to register
                }
                state = FETCH_OP;
                break;
            case FETCH_OP: // Look at ST. Microstate 23 example of getting a value out of a register
                switch (opcode) {
                    case ADD:
                            alu->A = cpu->regFile[Rs1];
                            alu->B = cpu->regFile[Rs2];
                            break;
                    // get operands out of registers into A, B of ALU
                    // or get memory for load instr.
                }
                state = EXECUTE;
                break;
            case EXECUTE: // Note that ST does not have an execute microstate
                switch (opcode) {
                    // do what the opcode is for, e.g. ADD
                    case ADD:
                            alu->R = alu->A + alu->B;
                            break;
                    // in case of TRAP: call trap(int trap_vector) routine, see below for TRAP x25 (HALT)
                }
                state = STORE;
                break;
            case STORE: // Look at ST. Microstate 16 is the store to memory
                switch (opcode) {
                    case ADD:
                            cpu->regFile[Rd] = alu->R;
                            break;
                    // write back to register or store MDR into memory
                }
printf("RESULT: %d", cpu->regFile[Rd]);
exit(0);
                // do any clean up here in prep for the next complete cycle
                state = FETCH;
                break;
        }
    }

    
}


int main(int argc, char* argv[]) {
    CPU_p cpu_pointer = malloc(sizeof(struct CPU_s));
    cpu_pointer->PC = 0;
    cpu_pointer->regFile[1] = 1;
    cpu_pointer->regFile[2] = 5;
    memory[0] = 0x1642;
    controller(cpu_pointer);
}