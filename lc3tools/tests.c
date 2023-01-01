#include <stdio.h>
#include <stdlib.h>

int a_output[100];
int idx = 0;

typedef enum ccode_t ccode_t;
enum ccode_t {
    CC_    = 0,
    CC_P   = 0x0200,
    CC_Z   = 0x0400,
    CC_N   = 0x0800
};

typedef enum operands_t operands_t;
enum operands_t {
    O_RRR, O_RRI,
    O_RR,  O_RI,  O_RL,
    O_R,   O_I,   O_L,   O_S,
    O_,
    NUM_OPERANDS
};

typedef enum opcode_t opcode_t;
enum opcode_t {
    /* no opcode seen (yet) */
    OP_NONE,

    /* real instruction opcodes */
    OP_ADD, OP_AND, OP_BR, OP_DEC, OP_INC, OP_JMP, OP_JSR, OP_JSRR, OP_LD, OP_LDI, OP_LDR,
    OP_LEA, OP_MLT, OP_MOV, OP_NEG, OP_NOT, OP_OR, OP_RST, OP_RTI, OP_ST, OP_STI, OP_STR, OP_SUB, OP_TRAP,

    /* trap pseudo-ops */
    OP_GETC, OP_HALT, OP_IN, OP_OUT, OP_PUTS, OP_PUTSP,

    /* non-trap pseudo-ops */
    OP_FILL, OP_RET, OP_STRINGZ,

    /* directives */
    OP_BLKW, OP_END, OP_ORIG, 

    NUM_OPS
};

typedef struct inst_t inst_t;
struct inst_t {
    opcode_t op;
    ccode_t  ccode;
};

inst_t inst;
operands_t operands;
const unsigned char* o3;
int val;
int line_num = 0;
int num_errors = 0;
int tempA, tempB = 0;


void write_value(int);
void subtract(operands_t operands, int r1, int r2, int r3, const char* o3);
int read_val (const char* s, int* vptr, int bits);
void test_sub_dist_registers();
void test_sub_dist_immediate();

int main(void) {
    test_sub_dist_registers();
    test_sub_dist_immediate();
}

void test_sub_dist_registers() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    int r3 = 2;
    const char* imm = "0x0000";
    operands_t operands = O_RRR;
    subtract(operands, r1, r2, r3, imm);

    printf("Test for subtracting with distinct registers: ");
    printf( (a_output[0]==0x3601) && \
            (a_output[1]==0x0E01) && \
            (a_output[2]==0x0000) && \
            (a_output[3]==0x56E0) && \
            (a_output[4]==0x16C2) && \
            (a_output[5]==0x96FF) && \
            (a_output[6]==0x16E1) && \
            (a_output[7]==0x1043) && \
            (a_output[8]==0x27F9) && \
            (a_output[9]==0x1020) ? "PASSED\n" : "FAILED\n");
}

void test_sub_dist_immediate() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    int r3 = 0;
    const char* imm = "#2";
    operands_t operands = O_RRI;
    subtract(operands, r1, r2, r3, imm);

    printf("Test for subtracting with immediate value: ");
    printf ((a_output[0]==0x3401) && \
            (a_output[1]==0x0E01) && \
            (a_output[2]==0x0000) && \
            (a_output[3]==0x54A0) && \
            (a_output[4]==0x14A2) && \
            (a_output[5]==0x94BF) && \
            (a_output[6]==0x14A1) && \
            (a_output[7]==0x1042) && \
            (a_output[8]==0x25F9) && \
            (a_output[9]==0x1020) ? "PASSED\n" : "FAILED\n");
}

void subtract(operands_t operands, int r1, int r2, int r3, const char* o3) {
    #include "sub.h"
}

int read_val (const char* s, int* vptr, int bits)
{
    char* trash;
    long v;

    if (*s == 'x' || *s == 'X')
	v = strtol (s + 1, &trash, 16);
    else {
	if (*s == '#')
	    s++;
	v = strtol (s, &trash, 10);
    }
    if (0x10000 > v && 0x8000 <= v)
        v |= -65536L;   /* handles 64-bit longs properly */
    if (v < -(1L << (bits - 1)) || v >= (1L << bits)) {
	fprintf (stderr, "%3d: constant outside of allowed range\n", line_num);
	num_errors++;
	return -1;
    }
    if ((v & (1UL << (bits - 1))) != 0)
	v |= ~((1UL << bits) - 1);
    *vptr = v;
    return 0;
}

void write_value (int val)
{
    a_output[idx] = val;
    idx++;
}