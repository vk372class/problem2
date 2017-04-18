#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lc3.h"

// you can define a simple memory module here for this program
unsigned short memory[32]; // 32 words of memory enough to store simple program

//Sets the condition codes, given a result.
void setCC(unsigned short result, CPU_p cpu) {
    if (result < 0) { //Negative result
        cpu->CC = N;
    } else if (result == 0) { //Result = 0
        cpu->CC = Z;
    } else { //Positive result
        cpu->CC = P;
    }
}

//Prints out the register values, the IR, PC, MAR, and MDR.
void printCurrentState(CPU_p cpu) {
  int i;
  int numOfRegisters = sizeof(cpu->regFile)/sizeof(cpu->regFile[0]);
  printf("Registers: ");
  for (i = 0; i < numOfRegisters; i++) {
    printf("R%d: %d | ", i, cpu->regFile[i]);
  }
  printf("\nIR: %d\nPC: %d\nMAR: %d\nMDR: %d", cpu->IR, cpu->PC, cpu->MAR, cpu->MDR);
}

//Function to handle TRAP routines.
int trap(int trap_vector) {
    switch(trap_vector) {
        case 25: //HALT
            return 0;
    }
}

//Executes instructions on our simulated CPU.
int controller(CPU_p cpu) {
    if (cpu == NULL) {
      return -1;
    }
    
    unsigned short opcode, Rd, Rs1, Rs2, immed_offset, nzp, BEN, pcOffset9; // fields for the IR
    int state = FETCH;
    for (;;) { // efficient endless loop
        switch (state) {
            case FETCH: // microstates 18, 33, 35 in the book
                cpu->MAR = cpu->PC;
                cpu->PC++; // increment PC
                cpu->MDR = memory[cpu->MAR];
                cpu->IR = cpu->MDR;
    
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                #if DEBUG == 1
                printf("\n===========FETCH==============\n");
                printCurrentState(cpu);
                #endif
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                state = DECODE;
                break;
            case DECODE: // microstate 32
                // get the fields out of the IR
                opcode = cpu->IR & 0xF000;
                opcode = opcode >> 12;
                Rd = cpu->IR & 0x0E00;
                Rd = Rd >> 9;
                nzp = Rd;
                Rs1 = cpu->IR & 0x01C0;
                Rs1 = Rs1 >> 6;
                Rs2 = cpu->IR & 0x0007;
                BEN = cpu->CC & nzp; //current cc & instruction's nzp
                pcOffset9 = 0x01FF & cpu->IR;
                if (pcOffset9 & 0x0100) {
                    pcOffset9 = pcOffset9 | 0xFE00;
                }
    
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                #if DEBUG == 1
                printf("\n===========DECODE==============\n");
                printCurrentState(cpu);
                #endif
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                
                state = EVAL_ADDR;
                break;
            case EVAL_ADDR:
                switch (opcode) {
                    case TRAP:
                        cpu->MAR = cpu->IR & 0x00FF; //get the trapvector8
                        break;
                    case LD:
                    case ST:
                        cpu->MAR = cpu->PC + pcOffset9;
                        break;
                    case BR:
                        if (BEN) {
                            cpu->PC = cpu->PC + pcOffset9;
                        }
                        break;
                    default:
                        break;
                }
                    
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                #if DEBUG == 1
                printf("\n===========EVAL_ADDR==============\n");
                printCurrentState(cpu);
                #endif
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                
                state = FETCH_OP;
                break;
            case FETCH_OP:
                switch (opcode) {
                    case ADD:
                    case AND:
                        cpu->ALU_A = cpu->regFile[Rs1];
                        if ((cpu->IR & 0x20) == 0) {
                            cpu->ALU_B = cpu->regFile[Rs2];
                        } else {
                            cpu->ALU_B = cpu->IR & 0x1F; //get immed5.
                            if ((cpu->ALU_B & 0x10) != 0) { //if first bit of immed5 = 1
                                cpu->ALU_B = (cpu->ALU_B | 0xFFE0);
                            }
                        }
                        break;
                    case NOT:
                        cpu->ALU_A = cpu->regFile[Rs1];
                        break;
                    case LD:
                        cpu->MDR = memory[cpu->MAR];
                        break;
                    case ST:
                        cpu->MDR = Rd;
                        break;
                    default:
                        break;
                    }
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                #if DEBUG == 1
                printf("\n===========FETCH_OP==============\n");
                printCurrentState(cpu);
                #endif
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                
                state = EXECUTE;
                break;
            case EXECUTE:
                switch (opcode) {
                    case ADD:
                        cpu->ALU_R = cpu->ALU_A + cpu->ALU_B;
                        setCC(cpu->ALU_R, cpu);
                        break;
                    case AND:
                        cpu->ALU_R = cpu->ALU_A & cpu->ALU_B;
                        setCC(cpu->ALU_R, cpu);
                        break;
                    case NOT:
                        cpu->ALU_R = ~(cpu->ALU_A);
                        setCC(cpu->ALU_R, cpu);
                        break;
                    case TRAP:
                        return trap(cpu->MAR);
                        break;
                    case JMP:
                        cpu->PC = cpu->regFile[Rs1];
                        break;
                    default:
                        break;
                }    
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                #if DEBUG == 1
                printf("\n===========EXECUTE==============\n");
                printCurrentState(cpu);
                #endif
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                
                state = STORE;
                break;
            case STORE:
                switch (opcode) {
                    case ADD:
                        cpu->regFile[Rd] = cpu->ALU_R;
                        break;
                    case AND:
                        cpu->regFile[Rd] = cpu->ALU_R;
                        break;
                    case NOT:
                        cpu->regFile[Rd] = cpu->ALU_R;
                        break;
                    case LD:
                        cpu->regFile[Rd] = cpu->MDR;
                        setCC(cpu->regFile[Rd], cpu);
                        break;
                    case ST:
                        memory[cpu->MAR] = cpu->MDR;
                        break;
                    default:
                        break;
                }
                
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                #if DEBUG == 1
                printf("\n===========STORE==============\n");
                printCurrentState(cpu);
                #endif
                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                
                state = FETCH;
                break;
        }
    }

}

//Initializes the CPU and sets it into action.
int main(int argc, char * argv[]) {
    CPU_p cpu_pointer = malloc(sizeof(struct CPU_s));
    cpu_pointer->PC = 0;
    cpu_pointer->CC = Z; //initialize condition code to zero.
    cpu_pointer->regFile[0] = 0x1E;    
    cpu_pointer->regFile[1] = 0x5;
    cpu_pointer->regFile[2] = 0xF;
    cpu_pointer->regFile[3] = 0;
 
    char *temp;
    memory[0] = strtol(argv[1], &temp, 16);
    memory[1] = HALT; //TRAP #25
    memory[5] = 0xA0A0; //"You will need to put a value in location 4 - say 0xA0A0"
    memory[21] = 0x16A6; //ADD R3 R2 #6
    memory[22] = HALT; //TRAP #25
    memory[30] = 0x1672; //ADD R3 R1 #-14
    memory[31] = HALT; //TRAP #25
    controller(cpu_pointer);
    
    printf("\n===========HALTED==============\n");
    printCurrentState(cpu_pointer);
    printf("\n");
    
    /**
    ADD R3 R1 R2       0x1642
    ADD R3, R1, #2     0x1662
    AND R3, R1, R2     0x5642
    AND R3, R1, #15    0x566F
    NOT R3, R1         0x967F
    TRAP #25           0xF019
    LD R0, 0x0004      0x2004
    JMP R0             0xC000
    BRnzp #20          0x0E14
    */
    
    return 0;
}