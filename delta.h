#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

/********
Registers
********/
int r[32];
int ctl[6];

/*****
Memory
*****/
typedef struct memcell
{
	unsigned int address;
	unsigned char byte;
	struct memcell* next;
}memcell;
memcell* memhash[16];

/***************
Parser Functions
***************/
int reg(char*);
int imm(char*);

void directive(char*);

void label_add(unsigned int, char*);
unsigned int* label_find(char*);

/*************
Label Datatype
*************/
typedef struct label
{
	char* caption;
	unsigned int value;
	struct label* next;
}label;
label* label_head;

/*****************************
Link Between Parser & Emulator
*****************************/
typedef struct instruction
{
	char oper[10];
	int op1;
	int op2;
	int op3;
	int immed;
}instruction;

/************************
Emulator Helper Functions
************************/
void init();
int gtimm5(unsigned char);
unsigned int bits4to0(int);

void mem_add(unsigned int, unsigned char);
unsigned char mem_find(unsigned int);

void printer3r(char*, int, int, int, int);
void printer2rimm(char*, int, int, short, int);
void printer2rimmu(char*, int, int, unsigned short, int);
void printer2rimm5(char*, int, int, unsigned char, int);
char* brackets(char);

int valid3r(int, int, int);

int valid_r(int);

/*******
Emulator
*******/
void emulator(struct instruction*);

void add	(int, int, int);
void addi	(int, int, short);
void and	(int, int, int);
void andhi	(int, int, unsigned short);
void andi	(int, int, unsigned short);

void cmpeq	(int, int, int);
void cmpeqi	(int, int, short);

void cmpge	(int, int, int);
void cmpgei	(int, int, short);
void cmpgeu	(int, int, int);
void cmpgeui	(int, int, unsigned short);

void cmpgt	(int, int, int);
void cmpgti	(int, int, short);
void cmpgtu	(int, int, int);
void cmpgtui	(int, int, unsigned short);

void cmple	(int, int, int);
void cmplei	(int, int, short);
void cmpleu	(int, int, int);
void cmpleui	(int, int, unsigned short);

void cmplt	(int, int, int);
void cmplti	(int, int, short);
void cmpltu	(int, int, int);
void cmpltui	(int, int, unsigned short);

void cmpne	(int, int, int);
void cmpnei	(int, int, short);

void myDiv	(int, int, int);
void divu	(int, int, int);

void ldb	(int, short, int);
void ldbu	(int, short, int);
void ldh	(int, short, int);
void ldhu	(int, short, int);
void ldw	(int, short, int);

void mov	(int, int);
void movhi	(int, unsigned short);
void movi	(int, short);
void movia	(int, unsigned int);
void movui	(int, unsigned short);

void mul	(int, int, int);
void muli	(int, int, short);
void mulxss	(int, int, int);
void mulxsu	(int, int, int);
void mulxuu	(int, int, int);

void nop	();

void nor	(int, int, int);
void or		(int, int, int);
void orhi	(int, int, unsigned short);
void ori	(int, int, unsigned short);

void rdctl	(int, int);

void rol	(int, int, int);
void roli	(int, int, unsigned char);
void ror	(int, int, int);

void sll	(int, int, int);
void slli	(int, int, unsigned char);
void sra	(int, int, int);
void srai	(int, int, unsigned char);
void srl	(int, int, int);
void srli	(int, int, unsigned char);

void stb	(int, short, int);
void sth	(int, short, int);
void stw	(int, short, int);

void sub	(int, int, int);
void subi	(int, int, short);

void wrctl	(int, int);

void xor	(int, int, int);
void xorhi	(int, int, unsigned short);
void xori	(int, int, unsigned short);

 void info  ();     ///Doesn't exist in NIOS, acts like the variables table showing their values.

void blackjack	();
void card(int*, int*, int*);
