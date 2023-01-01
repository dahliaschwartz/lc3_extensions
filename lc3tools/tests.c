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

void test_dec();
void test_inc();
void test_mlt_registers();
void test_mlt_immediate();
void test_mov_dif_registers();
void test_mov_same_register();
void test_neg_dif_registers();
void test_neg_same_register();
void test_or_registers();
void test_or_immediate();
void test_rst();
void test_sub_registers();
void test_sub_immediate();

void dec(int r1);
void inc(int r1);
void mlt(operands_t operands, int r1, int r2, int r3, const char* o3);
void mov(int r1, int r2);
void neg(int r1, int r2);
void or(operands_t operands, int r1, int r2, int r3, const char* o3);
void rst(int r1);
void sub(operands_t operands, int r1, int r2, int r3, const char* o3);

int read_val (const char* s, int* vptr, int bits);
void write_value(int);


int main(void) {
    test_dec();
    test_inc();
    test_mlt_registers();
    test_mlt_immediate();
    test_mov_dif_registers();
    test_mov_same_register();
    test_neg_dif_registers();
    test_neg_same_register();
    test_or_registers();
    test_or_immediate();
    test_rst();
    test_sub_registers();
    test_sub_immediate();
}

void test_dec() {
    idx = 0;
    int r1 = 0;
    dec(r1);

    printf(a_output[0]==0x103F ? "PASSED: " : "FAILED: ");
    printf("Decrement\n");
}

void test_inc() {
    idx = 0;
    int r1 = 0;
    inc(r1);

    printf(a_output[0]==0x1021 ? "PASSED: " : "FAILED: ");
    printf("Increment\n");
}

void test_mlt_registers() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    int r3 = 2;
    const char* imm = "0x0000";
    operands_t operands = O_RRR;
    mlt(operands, r1, r2, r3, imm);

    printf( (a_output[0]==0x3602) && \
            (a_output[1]==0x3802) && \
            (a_output[2]==0x0E02) && \
            (a_output[3]==0x0000) && \
            (a_output[4]==0x0000) && \
            (a_output[5]==0x56E0) && \
            (a_output[6]==0x16C1) && \
            (a_output[7]==0x5920) && \
            (a_output[8]==0x1902) && \
            (a_output[9]==0x5020) && \
            (a_output[10]==0x1920) && \
            (a_output[11]==0x0804) && \
            (a_output[12]==0x1003) && \
            (a_output[13]==0x193F) && \
            (a_output[14]==0x0BFD) && \
            (a_output[15]==0x0E05) && \
            (a_output[16]==0x1003) && \
            (a_output[17]==0x1921) && \
            (a_output[18]==0x0BFD) && \
            (a_output[19]==0x903F) && \
            (a_output[20]==0x1021) && \
            (a_output[21]==0x27ED) && \
            (a_output[22]==0x29ED) && \
            (a_output[23]==0x1020) ? "PASSED: " : "FAILED: ");
    printf("Multiply with three registers\n");
}

void test_mlt_immediate() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    int r3 = 0;
    const char* imm = "#2";
    operands_t operands = O_RRI;
    mlt(operands, r1, r2, r3, imm);

    printf( (a_output[0]==0x3402) && \
            (a_output[1]==0x3602) && \
            (a_output[2]==0x0E02) && \
            (a_output[3]==0x0000) && \
            (a_output[4]==0x0000) && \
            (a_output[5]==0x54A0) && \
            (a_output[6]==0x1481) && \
            (a_output[7]==0x56E0) && \
            (a_output[8]==0x16E2) && \
            (a_output[9]==0x5020) && \
            (a_output[10]==0x16E0) && \
            (a_output[11]==0x0804) && \
            (a_output[12]==0x1002) && \
            (a_output[13]==0x16FF) && \
            (a_output[14]==0x0BFD) && \
            (a_output[15]==0x0E05) && \
            (a_output[16]==0x1002) && \
            (a_output[17]==0x16E1) && \
            (a_output[18]==0x0BFD) && \
            (a_output[19]==0x903F) && \
            (a_output[20]==0x1021) && \
            (a_output[21]==0x25ED) && \
            (a_output[22]==0x27ED) && \
            (a_output[23]==0x1020) ? "PASSED: " : "FAILED: ");
    printf("Multiply with immediate value\n");
}

void test_mov_dif_registers() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    mov(r1, r2);

    printf( (a_output[0]==0x5020) && \
            (a_output[1]==0x1001) ? "PASSED: " : "FAILED: ");
    printf("Move from one register to another\n");
}

void test_mov_same_register() {
    idx = 0;
    int r1 = 0;
    int r2 = 0;
    mov(r1, r2);

    printf(a_output[0]==0x1020 ? "PASSED: " : "FAILED: ");
    printf("Move within the same register\n");
}

void test_neg_dif_registers() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    neg(r1, r2);

    printf( (a_output[0]==0x907F) && \
            (a_output[1]==0x1021) ? "PASSED: " : "FAILED: ");
    printf("Negating from one register to another\n");
}

void test_neg_same_register() {
    idx = 0;
    int r1 = 0;
    int r2 = 0;
    neg(r1, r2);

    printf( (a_output[0]==0x903F) && \
            (a_output[1]==0x1021) ? "PASSED: " : "FAILED: ");
    printf("Negating within a register\n");
}

void test_or_registers() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    int r3 = 2;
    const char* imm = "0x0000";
    operands_t operands = O_RRR;
    or(operands, r1, r2, r3, imm);

    printf( (a_output[0]==0x3601) && \
            (a_output[1]==0x0E01) && \
            (a_output[2]==0x0000) && \
            (a_output[3]==0x56E0) && \
            (a_output[4]==0x16C2) && \
            (a_output[5]==0x907F) && \
            (a_output[6]==0x96FF) && \
            (a_output[7]==0x5003) && \
            (a_output[8]==0x903F) && \
            (a_output[9]==0x27F8) && \
            (a_output[10]==0x1020) ? "PASSED: " : "FAILED: ");
    printf("Or with three registers\n");
}

void test_or_immediate() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    int r3 = 0;
    const char* imm = "#2";
    operands_t operands = O_RRI;
    or(operands, r1, r2, r3, imm);

    printf ((a_output[0]==0x3401) && \
            (a_output[1]==0x0E01) && \
            (a_output[2]==0x0000) && \
            (a_output[3]==0x54A0) && \
            (a_output[4]==0x14A2) && \
            (a_output[5]==0x907F) && \
            (a_output[6]==0x94BF) && \
            (a_output[7]==0x5002) && \
            (a_output[8]==0x903F) && \
            (a_output[9]==0x25F8) && \
            (a_output[10]==0x1020) ? "PASSED: " : "FAILED: ");
    printf("Or with immediate value\n");
}

void test_rst() {
    idx = 0;
    int r1 = 0;
    rst(r1);

    printf(a_output[0]==0x5020 ? "PASSED: " : "FAILED: ");
    printf("Reset\n");
}

void test_sub_registers() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    int r3 = 2;
    const char* imm = "0x0000";
    operands_t operands = O_RRR;
    sub(operands, r1, r2, r3, imm);

    printf( (a_output[0]==0x3601) && \
            (a_output[1]==0x0E01) && \
            (a_output[2]==0x0000) && \
            (a_output[3]==0x56E0) && \
            (a_output[4]==0x16C2) && \
            (a_output[5]==0x96FF) && \
            (a_output[6]==0x16E1) && \
            (a_output[7]==0x1043) && \
            (a_output[8]==0x27F9) && \
            (a_output[9]==0x1020) ? "PASSED: " : "FAILED: ");
    printf("Subtracting with three registers\n");
}

void test_sub_immediate() {
    idx = 0;
    int r1 = 0;
    int r2 = 1;
    int r3 = 0;
    const char* imm = "#2";
    operands_t operands = O_RRI;
    sub(operands, r1, r2, r3, imm);

    printf ((a_output[0]==0x3401) && \
            (a_output[1]==0x0E01) && \
            (a_output[2]==0x0000) && \
            (a_output[3]==0x54A0) && \
            (a_output[4]==0x14A2) && \
            (a_output[5]==0x94BF) && \
            (a_output[6]==0x14A1) && \
            (a_output[7]==0x1042) && \
            (a_output[8]==0x25F9) && \
            (a_output[9]==0x1020) ? "PASSED: " : "FAILED: ");
    printf("Subtracting with immediate value\n");
}

void dec(int r1) {
    #include "dec.h"
}

void inc(int r1) {
    #include "inc.h"
}

void mlt(operands_t operands, int r1, int r2, int r3, const char* o3) {
    #include "mlt.h"
}

void mov(int r1, int r2) {
    #include "mov.h"
}

void neg(int r1, int r2) {
    #include "neg.h"
}

void or(operands_t operands, int r1, int r2, int r3, const char* o3) {
    #include "or.h"
}

void rst(int r1) {
    #include "rst.h"
}

void sub(operands_t operands, int r1, int r2, int r3, const char* o3) {
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