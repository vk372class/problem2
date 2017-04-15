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
                    case AND:
                          break;
                    case NOT:
                          break;
                    case TRAP:
                            cpu->MAR = cpu->IR & 0x00FF;
                          break;
                    case LD:
                    case ST:
                            cpu->IR = 0x01FF & cpu->IR;  
                            if(cpu->IR & 0x0100) {
                                cpu->IR = cpu->IR | 0xFE00;
                            }
                            cpu->MAR = cpu->PC + cpu->IR;
                            break;
                    case JMP:
                          break;
                    case BR:
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
                        if ((cpu->IR & 0x20) == 0) {
                            alu->B = cpu->regFile[Rs2];
                        } else {
                          alu->B = cpu->IR & 0x1F; //get immed5.
                          if (alu->B & 0x10 != 0) { //if first bit of immed5 = 1
                            alu->B = (alu->B | 0xFFE0);
                          }
                        }
                            break;
                    case AND:
                          alu->A = cpu->regFile[Rs1];
                        if ((cpu->IR & 0x20) == 0) {
                            printf("here!\n");
                            alu->B = cpu->regFile[Rs2];
                        } else {
                          alu->B = cpu->IR & 0x1F; //get immed5.
                          printf("\nalub: %d", alu->B);
                        } 
                            break;
                    case NOT:
                          alu->A = cpu->regFile[Rs1];
                          break;
                    case TRAP:
                          break;
                    case LD:
                            cpu->MDR = memory[cpu->MAR];
                          break;
                    case ST:
                            cpu->MDR = Rd;
                          break;
                    case JMP:
                          break;
                    case BR:
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
                    case AND:
                          alu->R = alu->A & alu->B;
                          printf("\na: %d, \nb: %d\n", alu->A, alu->B);                          
                          break;
                    case NOT:
                          alu->R = ~(alu->A);
                          break;
                    case TRAP:
                          break;
                    case LD:
                          break;
                    case ST:
                          break;
                    case JMP:
                            cpu->PC = cpu->regFile[Rs1];
                          break;
                    case BR:
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
                    case AND:
                          cpu->regFile[Rd] = alu->R;
                          break;
                    case NOT:
                          cpu->regFile[Rd] = alu->R;
                          break;
                    case TRAP:
                          break;
                     case LD:
                            cpu->regFile[Rd] = cpu->MDR;
                          break;
                    case ST:
                            memory[cpu->MAR] = cpu->MDR;
                          break;
                    case JMP:
                          break;
                    case BR:
                          break;
                    // write back to register or store MDR into memory
                }
printf("\nRESULT: %d", cpu->regFile[Rd]);
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
    cpu_pointer->regFile[1] = 3;
    cpu_pointer->regFile[2] = 5;
    //memory[0] = 0x167D; //ADD R3 R1 #-3
    //memory[0] = 0x967F; //NOT R3 R1
    //memory[0] = 0x5642; //AND R3 R1 R2
    memory[0] = 0x5679;
    controller(cpu_pointer);
}
