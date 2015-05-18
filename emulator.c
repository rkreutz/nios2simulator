#include "delta.h"

void init()
{
  //printf ("Welcome to Delta-Nios\n\n");

	int i;

	for(i = 1; i < 32; i++)
		r[i] = 0x0;

	for(i = 0; i < 6; i++)
		ctl[i] = 0x0;

	for(i = 0; i < 16; i++)
		memhash[i] = NULL;
}

/***********************
Bit Operations Functions
***********************/
int gtimm5(unsigned char imm5)
{
	if (imm5 < 32)	return 0;
	printf("Error: IMMED value exceeds 5 bits\n");
	return 1;
}

unsigned int bits4to0(int x)
{
	return (x & 31);
}

/************************
Emulated Memory Functions
************************/
void mem_add(unsigned int address, unsigned char byte)
{
	int hash_index = address & 0x0000000f;
	memcell* index = memhash[hash_index];

	memcell* newmemcell = (memcell*) malloc(sizeof(memcell));
	newmemcell->address = address;
	newmemcell->byte = byte;
	newmemcell->next = NULL;

	if (index == NULL) //empty scenario
	{
		memhash[hash_index] = newmemcell;
		return;
	}

	memcell* last_index;
	while(index != NULL)
	{
		if (index->address == address)
		{
			index->byte = byte;
			return;
		}
		last_index = index;
		index = index->next;
	}
	last_index->next = newmemcell;
}

unsigned char mem_find(unsigned int address)
{
	int hash_index = address & 0x0000000f;
	memcell* index = memhash[hash_index];
	while(index != NULL)
	{
		if (index->address == address)
			return index->byte;
		index = index->next;
	}
	printf("Warning: Memory address 0x%x not initialized\n", address);
	return 0;
}

/****************
Printer Functions
****************/
void printer3r(char* oper_symbol, int c, int a, int b, int ans)
{
	char* bracket = brackets(oper_symbol[0]);
	printf("# r%d = %cr%d %s r%d%c = %c0x%x %s 0x%x%c = 0x%x\n",
	c, bracket[0], a, oper_symbol, b, bracket[1], bracket[0], r[a], oper_symbol, r[b], bracket[1], ans);
	r[c] = ans;
}

void printer2rimm(char* oper_symbol, int b, int a, short imm16, int ans)
{
	char* bracket = brackets(oper_symbol[0]);
	printf("# r%d = %cr%d %s SE32(0x%hx)%c = %c0x%x %s SE32(0x%hx)%c = 0x%x\n",
	b, bracket[0], a, oper_symbol, imm16, bracket[1], bracket[0], r[a], oper_symbol, imm16, bracket[1], ans);
	r[b] = ans;
}

void printer2rimmu(char* oper_symbol, int b, int a, unsigned short imm16, int ans)
{
	char* bracket = brackets(oper_symbol[0]);
	printf("# r%d = %cr%d %s ZE32(0x%hx)%c = %c0x%x %s ZE32(0x%hx)%c = 0x%x\n",
	b, bracket[0], a, oper_symbol, imm16, bracket[1], bracket[0], r[a], oper_symbol, imm16, bracket[1], ans);
	r[b] = ans;
}

void printer2rimm5(char* oper_symbol, int c, int a, unsigned char imm5, int ans)
{
	printf("# r%d = r%d %s 0x%hx = 0x%x %s 0x%hx = 0x%x\n",
	c, a, oper_symbol, imm5, r[a], oper_symbol, imm5, ans);
	r[c] = ans;
}

char* brackets(char oper_symbol)
{
	char* brackets = (char*) malloc(sizeof(char)*2);
	switch(oper_symbol)
	{
		case '=':
		case '>':
		case '<':
		case '!':
			brackets[0] = '(';
			brackets[1] = ')';
			break;
		default:
			brackets[0] = '\0';
			brackets[1] = '\0';
			break;
	}
	return brackets;
}

/*****************
Validity Functions
*****************/
int valid3r(int c, int a, int b)
{
	int valid = (valid_r(c) && valid_r(a) && valid_r(b));
	if (!valid) printf("Invalid registers!\n");
	return valid;
}

int valid_r(int r)
{
	return (r > -1 && r < 32);
}

/**********
Control Box
**********/
void emulator(struct instruction* instr)
{
	//pull apart the structure
	char* oper = instr->oper;
	int op1 = instr->op1;
	int op2 = instr->op2;
	int op3 = instr->op3;
	int immed = instr->immed;

	if (0) ;

	else if (!strcmp(oper,"add"))	add	(op1, op2, op3);
	else if (!strcmp(oper,"addi"))	addi	(op1, op2, immed);
	else if (!strcmp(oper,"and"))	and	(op1, op2, op3);
	else if (!strcmp(oper,"andhi"))	andhi	(op1, op2, immed);
	else if (!strcmp(oper,"andi"))	andi	(op1, op2, immed);

	else if (!strcmp(oper,"cmpeqi"))	cmpeqi	(op1, op2, immed);
	else if (!strcmp(oper,"cmpeq"))		cmpeq	(op1, op2, op3);
	else if (!strcmp(oper,"cmpgei")) 	cmpgei	(op1, op2, immed);
	else if (!strcmp(oper,"cmpgeui")) 	cmpgeui	(op1, op2, immed);
	else if (!strcmp(oper,"cmpgeu")) 	cmpgeu	(op1, op2, op3);
	else if (!strcmp(oper,"cmpge")) 	cmpge	(op1, op2, op3);
	else if (!strcmp(oper,"cmpgtui"))	cmpgtui	(op1, op2, immed);
	else if (!strcmp(oper,"cmpgtu"))	cmpgtu	(op1, op2, op3);
	else if (!strcmp(oper,"cmpgti"))	cmpgti	(op1, op2, immed);
	else if (!strcmp(oper,"cmpgt"))		cmpgt	(op1, op2, op3);
	else if (!strcmp(oper,"cmpleui")) 	cmpleui	(op1, op2, immed);
	else if (!strcmp(oper,"cmpleu")) 	cmpleu	(op1, op2, op3);
	else if (!strcmp(oper,"cmplei")) 	cmplei	(op1, op2, immed);
	else if (!strcmp(oper,"cmple")) 	cmple	(op1, op2, op3);
	else if (!strcmp(oper,"cmplti")) 	cmplti	(op1, op2, immed);
	else if (!strcmp(oper,"cmpltui")) 	cmpltui	(op1, op2, immed);
	else if (!strcmp(oper,"cmpltu")) 	cmpltu	(op1, op2, op3);
	else if (!strcmp(oper,"cmplt")) 	cmplt	(op1, op2, op3);
	else if (!strcmp(oper,"cmpnei"))	cmpnei	(op1, op2, immed);
	else if (!strcmp(oper,"cmpne"))		cmpne	(op1, op2, op3);

	else if (!strcmp(oper,"divu"))	divu	(op1, op2, op3);
	else if (!strcmp(oper,"div"))	myDiv	(op1, op2, op3);

	else if (!strcmp(oper,"ldbu") || !strcmp(oper,"ldbuio"))	ldbu	(op1, immed, op2);
	else if (!strcmp(oper,"ldb") || !strcmp(oper,"ldbio"))		ldb	(op1, immed, op2);
	else if (!strcmp(oper,"ldh") || !strcmp(oper,"ldhio"))		ldh	(op1, immed, op2);
	else if (!strcmp(oper,"ldhu") || !strcmp(oper,"ldhuio"))	ldhu	(op1, immed, op2);
	else if (!strcmp(oper,"ldw") || !strcmp(oper,"ldwio"))		ldw	(op1, immed, op2);

	else if (!strcmp(oper,"mov"))	mov	(op1, op2);
	else if (!strcmp(oper,"movhi"))	movhi	(op1, immed);
	else if (!strcmp(oper,"movi"))	movi	(op1, immed);
	else if (!strcmp(oper,"movia"))	movia	(op1, immed);
	else if (!strcmp(oper,"movui"))	movui	(op1, immed);

	else if (!strcmp(oper,"mul"))		mul	(op1, op2, op3);
	else if (!strcmp(oper,"muli"))		muli	(op1, op2, immed);
	else if (!strcmp(oper,"mulxss"))	mulxss	(op1, op2, op3);
	else if (!strcmp(oper,"mulxsu"))	mulxsu	(op1, op2, op3);
	else if (!strcmp(oper,"mulxuu"))	mulxuu	(op1, op2, op3);

	else if (!strcmp(oper,"nop"))	nop();

	else if (!strcmp(oper,"nor"))	nor	(op1, op2, op3);
	else if (!strcmp(oper,"or"))	or	(op1, op2, op3);
	else if (!strcmp(oper,"orhi"))	orhi	(op1, op2, immed);
	else if (!strcmp(oper,"ori"))	ori	(op1, op2, immed);

	else if (!strcmp(oper,"rdctl"))	rdctl	(op1, op2);

	else if (!strcmp(oper,"rol"))	rol	(op1, op2, op3);
	else if (!strcmp(oper,"roli"))	roli	(op1, op2, immed);
	else if (!strcmp(oper,"ror"))	ror	(op1, op2, op3);
	else if (!strcmp(oper,"sll"))	sll	(op1, op2, op3);
	else if (!strcmp(oper,"slli"))	slli	(op1, op2, immed);
	else if (!strcmp(oper,"sra"))	sra	(op1, op2, op3);
	else if (!strcmp(oper,"srai"))	srai	(op1, op2, immed);
	else if (!strcmp(oper,"srl"))	srl	(op1, op2, op3);
	else if (!strcmp(oper,"srli"))	srli	(op1, op2, immed);

	else if (!strcmp(oper,"stb") || !strcmp(oper,"stbio"))	stb	(op1, immed, op2);
	else if (!strcmp(oper,"sth") || !strcmp(oper,"sthio"))	sth	(op1, immed, op2);
	else if (!strcmp(oper,"stw") || !strcmp(oper,"stwio"))	stw	(op1, immed, op2);

	else if (!strcmp(oper,"sub"))	sub	(op1, op2, op3);
	else if (!strcmp(oper,"subi"))	subi	(op1, op2, immed);

	else if (!strcmp(oper,"wrctl"))	wrctl	(op1, op2);

	else if (!strcmp(oper,"xor"))	xor	(op1, op2, op3);
	else if (!strcmp(oper,"xorhi"))	xorhi	(op1, op2, immed);
	else if (!strcmp(oper,"xori"))	xori	(op1, op2, immed);

	else if (!strcmp(oper,"info"))  info();

	else printf("Invalid instruction: \"%s\"\n", oper);
}

/********************
Addition Instructions
********************/

void add(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] + r[b];
	printer3r("+", c, a, b, ans);
}

void addi(int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = r[a] + imm16;
	printer2rimm("+", b, a, imm16, ans);
}

/*********************
Logic AND Instructions
*********************/

void and(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] & r[b];
	printer3r("&", c, a, b, ans);
}

void andhi(int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int imm32 = imm16 << 16;
	printf("r%d = r%d & 0x%hx0000 = 0x%x & 0x%hx0000 = ", b, a, imm16, r[a], imm16);
	r[b] = r[a] & imm32;
	printf("0x%x\n", r[b]);
}

void andi(int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = r[a] & imm16;
	printer2rimmu("&", b, a, imm16, ans);
}

/**********************
Comparison Instructions
**********************/

/**************
== Instructions
**************/

void cmpeq(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (r[a] == r[b]);
	printer3r("==", c, a, b, ans);
}
void cmpeqi(int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (r[a] == imm16);
	printer2rimm("==", b, a, imm16, ans);
}

/**************
>= Instructions
**************/

void cmpge (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (r[a] >= r[b]);
	printer3r(">=", c, a, b, ans);
}
void cmpgei (int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (r[a] >= imm16);
	printer2rimm(">=", b, a, imm16, ans);
}
void cmpgeu (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (((unsigned int)r[a]) >= ((unsigned int)r[b]));
	printer3r(">=", c, a, b, ans);
}
void cmpgeui (int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (((unsigned int)r[a]) >= imm16);
	printer2rimmu(">=", b, a, imm16, ans);
}

/**************
> Instructions
**************/

void cmpgt (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (r[a] > r[b]);
	printer3r(">", c, a, b, ans);
}
void cmpgti (int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (r[a] > imm16);
	printer2rimm(">", b, a, imm16, ans);
}
void cmpgtu (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (((unsigned int)r[a]) > ((unsigned int)r[b]));
	printer3r(">", c, a, b, ans);
}
void cmpgtui (int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (((unsigned int)r[a]) > imm16);
	printer2rimmu(">", b, a, imm16, ans);
}

/**************
<= Instructions
**************/

void cmple (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (r[a] <= r[b]);
	printer3r("<=", c, a, b, ans);
}
void cmplei (int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (r[a] <= imm16);
	printer2rimm("<=", b, a, imm16, ans);
}
void cmpleu (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (((unsigned int)r[a]) <= ((unsigned int)r[b]));
	printer3r("<=", c, a, b, ans);
}
void cmpleui (int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (((unsigned int)r[a]) <= imm16);
	printer2rimmu("<=", b, a, imm16, ans);
}

/**************
< Instructions
**************/

void cmplt (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (r[a] < r[b]);
	printer3r("<", c, a, b, ans);
}
void cmplti (int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (r[a] < imm16);
	printer2rimm("<", b, a, imm16, ans);
}
void cmpltu (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	unsigned ua = (unsigned int)r[a];
	unsigned ub = (unsigned int)r[b];
	printf("%u < %u\n",ua,ub);
	int ans = (((unsigned int)r[a]) < ((unsigned int)r[b]));
	printer3r("<", c, a, b, ans);
}
void cmpltui (int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (((unsigned int)r[a]) < imm16);
	printer2rimmu("<", b, a, imm16, ans);
}

/**************
!= Instructions
**************/

void cmpne (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = (r[a] != r[b]);
	printer3r("!=", c, a, b, ans);
}
void cmpnei (int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = (r[a] != imm16);
	printer2rimm("!=", b, a, imm16, ans);
}

/********************
Division Instructions
********************/

void myDiv (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] / r[b];
	printer3r("/", c, a, b, ans);
}

void divu (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = ((unsigned int)r[a]) / ((unsigned int)r[b]);
	printer3r("/", c, a, b, ans);
}

/***********************
Memory Load Instructions
***********************/

void ldb(int b, short byte_offset, int a)
{
        unsigned char v;
	if (!valid3r(b,1,a)) return;
        v = mem_find(r[a] + byte_offset);
	int ans = (signed char) v;

	printf("# r%d = SE32(mem[SE32(0x%04x) + 0x%x]) = SE32(mem[0x%x]) = SE32(0x%02x)= 0x%08x\n", b, byte_offset, r[a], byte_offset + r[a], v, ans);

	r[b] = ans;
}

void ldbu(int b, short byte_offset, int a)
{
	if (!valid3r(b,1,a)) return;
	int ans = mem_find(r[a] + byte_offset);

	printf("# r%d = ZE32(mem[SE32(0x%x) + 0x%x]) = ZE32(mem[0x%x]) = 0x%08x\n", b, byte_offset, r[a], byte_offset + r[a], ans);

	r[b] = ans;
}

void ldh(int b, short byte_offset, int a)
{
	if (!valid3r(b,1,a)) return;
	unsigned char byte0 = mem_find(r[a] + byte_offset);
	unsigned char ubyte1 = mem_find(r[a] + byte_offset + 1);
	char byte1 = mem_find(r[a] + byte_offset + 1);

	int uans;
	uans = ubyte1;
	uans = uans << 8;
	uans = uans + byte0;

	int ans;
	ans = byte1;
	ans = ans << 8;
	ans = ans + byte0;

	printf("# r%d = SE32(mem[SE32(0x%x) + 0x%x]) = SE32(mem[0x%x]) = SE32(0x%04x) = 0x%08x\n", b, byte_offset, r[a], byte_offset + r[a], uans, ans);

	r[b] = ans;
}

void ldhu(int b, short byte_offset, int a)
{
	if (!valid3r(b,1,a)) return;
	unsigned char byte0 = mem_find(r[a] + byte_offset);
	unsigned char byte1 = mem_find(r[a] + byte_offset + 1);
	int ans;
	ans = byte1;
	ans = ans << 8;
	ans = ans + byte0;

	printf("# r%d = ZE32(mem[SE32(0x%x) + 0x%x]) = ZE32(mem[0x%x]) = 0x%08x\n", b, byte_offset, r[a], byte_offset + r[a], ans);

	r[b] = ans;
}

void ldw(int b, short byte_offset, int a)
{
	if (!valid3r(b,1,a)) return;
	unsigned char byte0 = mem_find(r[a] + byte_offset);
	unsigned char byte1 = mem_find(r[a] + byte_offset + 1);
	unsigned char byte2 = mem_find(r[a] + byte_offset + 2);
	char byte3 = mem_find(r[a] + byte_offset + 3);
	int ans;
	ans = byte3;
	ans = ans << 8;
	ans = ans + byte2;
	ans = ans << 8;
	ans = ans + byte1;
	ans = ans << 8;
	ans = ans + byte0;

	printf("# r%d = mem[SE32(0x%x) + 0x%x] = mem[0x%x] = 0x%08x\n", b, byte_offset, r[a], byte_offset + r[a], ans);

	r[b] = ans;
}

void mov (int c, int a)
{
	if (!valid3r(c,a,1)) return;
	r[c] = r[a];
	printf("# r%d = r%d = 0x%x\n", c, a, r[a]);
}

void movhi (int b, unsigned short imm16)
{
	if (!valid3r(b,1,1)) return;
	printf("r%d = 0x%hx0000\n", b, imm16);
	r[b] = imm16 << 16;
}

void movi (int b, short imm16)
{
	if (!valid3r(b,1,1)) return;
	r[b] = imm16;
	printf("# r%d = SE32(0x%hx) = 0x%x\n", b, imm16, r[b]);
}

void movia (int b, unsigned int function_address)
{
	if (!valid3r(b,1,1)) return;
	r[b] = function_address;
	printf("# r%d = 0x%x\n", b, function_address);
}

void movui (int b, unsigned short imm16)
{
	if (!valid3r(b,1,1)) return;
	r[b] = imm16;
	printf("# r%d = ZE32(0x%hx) = 0x%x\n", b, imm16, r[b]);
}

/**************************
Multiplication Instructions
**************************/

void mul(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] * r[b];
	printer3r("*", c, a, b, ans);
}

void muli(int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = r[a] * imm16;
	printer2rimm("*", b, a, imm16, ans);
}

void mulxss(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	long long product = (long long)r[a] * r[b];
	printf("# r%d = HI32(r%d * r%d) = HI32(0x%x * 0x%x) = HI32(0x%llx) = ", c, a, b, r[a], r[b], product);
	product = product >> 32;
	r[c] = product;
	printf("0x%x\n", r[c]);
}

void mulxsu(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	long long product = r[a] * ((unsigned int) r[b]);
	printf("# r%d = HI32(r%d * r%d) = HI32(0x%x * 0x%x) = HI32(0x%llx) = ", c, a, b, r[a], r[b], product);
	product = product >> 32;
	r[c] = product;
	printf("0x%x\n", r[c]);
}

void mulxuu(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	long long product = ((unsigned int) r[a]) * ((unsigned int) r[b]);
	printf("# r%d = HI32(r%d * r%d) = HI32(0x%x * 0x%x) = HI32(0x%llx) = ", c, a, b, r[a], r[b], product);
	product = product >> 32;
	r[c] = product;
	printf("0x%x\n", r[c]);
}

//nop - does nothing
void nop()
{
	printf("# does nothing\n");
}

/********************
Logic OR Instructions
********************/

void nor(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = !(r[a] | r[b]);
	printer3r("nor", c, a, b, ans);
}

void or(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] | r[b];
	printer3r("|", c, a, b, ans);
}

void orhi(int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int imm32 = imm16 << 16;
	printf("r%d = r%d | 0x%hx0000 = 0x%x | 0x%hx0000 = ", b, a, imm16, r[a], imm16);
	r[b] = r[a] | imm32;
	printf("0x%x\n", r[b]);
}

void ori(int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = r[a] | imm16;
	printer2rimmu("|", b, a, imm16, ans);
}

//read control reg
void rdctl(int c, int n)
{
	if (!valid3r(c,1,1) || n < 0 || n > 6) return;
	r[c] = ctl[n];
	printf("# r%d = ctl%d = 0x%x\n", c, n, r[c]);
}

/********************
Rotating Instructions
********************/

void rol (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int roll = bits4to0(r[b]);
	unsigned int x = ((unsigned int)r[a]) >> (32-roll);
	int ans = r[a] << roll;
	ans = ans | x;
	printer3r("rotl", c, a, b, ans);
}

void roli (int c, int a, unsigned char imm5)
{
	if (!valid3r(c,a,1)) return;
	if(gtimm5(imm5)) return;
	unsigned int x = ((unsigned int)r[a]) >> (32-imm5);
	int ans = r[a] << imm5;
	ans = ans | x;
	printer2rimm5("rotl", c, a, imm5, ans);
}

void ror (int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int roll = bits4to0(r[b]);
	unsigned int x = r[a];
	x = x << (32-roll);
	int ans = ((unsigned int)r[a]) >> roll;
	ans = ans | x;
	printer3r("rotr", c, a, b, ans);
}

/********************
Shifting Instructions
********************/

void sll(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] << bits4to0(r[b]);
	printer3r("<<", c, a, b, ans);
}
void slli(int c, int a, unsigned char imm5)
{
	if (!valid3r(c,a,1)) return;
	if(gtimm5(imm5)) return;
	int ans = r[a] << imm5;
	printer2rimm5("rotl", c, a, imm5, ans);
}
void sra(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] >> bits4to0(r[b]);
	printer3r(">>", c, a, b, ans);
}
void srai(int c, int a, unsigned char imm5)
{
	if (!valid3r(c,a,1)) return;
	if(gtimm5(imm5)) return;
	int ans = r[a] >> imm5;
	printer2rimm5("rotl", c, a, imm5, ans);
}
void srl(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = ((unsigned int) r[a]) >> bits4to0(r[b]);
	printer3r(">>", c, a, b, ans);
}
void srli(int c, int a, unsigned char imm5)
{
	if (!valid3r(c,a,1)) return;
	if(gtimm5(imm5)) return;
	int ans = ((unsigned int) r[a]) >> imm5;
	printer2rimm5("rotl", c, a, imm5, ans);
}

/***********************
Subtraction Instructions
***********************/

void sub(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] - r[b];
	printer3r("-", c, a, b, ans);
}

void subi(int b, int a, short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = r[a] - imm16;
	printer2rimm("-", b, a, imm16, ans);
}

//write control reg
void wrctl(int n, int a)
{
	if (!valid3r(a,1,1) || n < 0 || n > 6) return;
	ctl[n] = r[a];
	printf("# ctl%d = r%d = 0x%x\n", n, a, r[a]);
}

/************************
Memory Store Instructions
************************/
void stb(int b, short byte_offset, int a)
{
	if (!valid3r(b,1,a)) return;
	unsigned char byte = r[b];
	mem_add(r[a] + byte_offset, byte);

	printf("# (mem[SE32(0x%x) + 0x%x] = mem[0x%x]) = r%d = 0x%x\n", byte_offset, r[a], byte_offset + r[a], b, byte);
}

void sth(int b, short byte_offset, int a)
{
	if (!valid3r(b,1,a)) return;
	unsigned char byte0 = r[b];
	mem_add(r[a] + byte_offset, byte0);
	unsigned char byte1 = r[b] >> 8;
	mem_add(r[a] + byte_offset + 1, byte1);

	printf("# (mem[SE32(0x%x) + 0x%x] = mem[0x%x]) = r%d = 0x%x%x\n", byte_offset, r[a], byte_offset + r[a], b, byte1, byte0);
}

void stw(int b, short byte_offset, int a)
{
	if (!valid3r(b,1,a)) return;
	unsigned char byte0 = r[b];
	mem_add(r[a] + byte_offset, byte0);
	unsigned char byte1 = r[b] >> 8;
	mem_add(r[a] + byte_offset + 1, byte1);
	unsigned char byte2 = r[b] >> 16;
	mem_add(r[a] + byte_offset + 2, byte2);
	unsigned char byte3 = r[b] >> 24;
	mem_add(r[a] + byte_offset + 3, byte3);

	printf("# (mem[SE32(0x%x) + 0x%x] = mem[0x%x]) = r%d = 0x%x\n", byte_offset, r[a], byte_offset + r[a], b, r[b]);
}

/*********************
Logic XOR Instructions
*********************/

void xor(int c, int a, int b)
{
	if (!valid3r(c,a,b)) return;
	int ans = r[a] ^ r[b];
	printer3r("^", c, a, b, ans);
}

void xorhi(int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int imm32 = imm16 << 16;
	printf("r%d = r%d ^ 0x%hx0000 = 0x%x ^ 0x%hx0000 = ", b, a, imm16, r[a], imm16);
	r[b] = r[a] ^ imm32;
	printf("0x%x\n", r[b]);
}

void xori(int b, int a, unsigned short imm16)
{
	if (!valid3r(b,a,1)) return;
	int ans = r[a] ^ imm16;
	printer2rimm("^", b, a, imm16, ans);
}

void info()
{
    char x;
    printf("Registers:\n");
    for (x = 0 ; x < 32 ; x++) {
        printf("r%d = 0x%X\n" , x , r[x]);
    }
}
