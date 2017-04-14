#ifndef LC3_H
#define LC3_H

#include <stdio.h>
#include <stdlib.h>

#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5

#define ADD 1

#define AND 5

#define NOT 9

#define TRAP 15

#define LD 2

#define ST 3

#define JMP 12

#define BR 0


typedef unsigned short Register;

typedef struct ALU_s {
    Register A;
    Register B;
    Register R;
} ALU_s;

typedef ALU_s *ALU_p;

typedef struct CPU_s {

    unsigned int regFile[8];
    int n, z, p;
    Register IR;
    Register PC;
    Register MAR;
    Register MDR;
 
} CPU_s;



typedef struct CPU_s *CPU_p;

#endif