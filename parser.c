#include "delta.h"

int main()
{
	init(); //inits the emulator

	while(1)
	{
		r[0] = 0;
		instruction instr;

		//set all default operands to fail condition
		instr.op1 = -1;
		instr.op2 = -1;
		instr.op3 = -1;
		instr.immed = 0;

		//printf("Delta-Nios> ");
		printf("> ");

		//scan in the operator
		scanf ("%s", instr.oper);
		if (!strcmp(instr.oper,"exit")) return 0;
		if (!strcmp(instr.oper,"quit")) return 0;

		//directive condition
		if (instr.oper[0] == '.')
		{
			directive(instr.oper);
			continue;
		}

		//scan in the operands
		char rawoperands [64000];
		fgets(rawoperands, 64000, stdin);

		//delete all whitespace from the operands and deals with case sensitivity
		int i = 0;
		int j = 0;
		char operands[strlen(rawoperands)];
		for (i = 0; i <= strlen(rawoperands); i++)
		{
			if (	rawoperands[i] != ' ' &&
				rawoperands[i] != '\r' &&
				rawoperands[i] != '\n' &&
				rawoperands[i] != '\t')
			{
				operands[j] = tolower(rawoperands[i]);
				j++;
			}
		}

		//tokenize the operands
  		char *token = NULL;
   		token = strtok(operands, ",");
		char* tokens [3];
		for(i = 0; i < 3; i++)
			tokens[i] = NULL;
   		for(i = 0; i < 3 && token != NULL; i++)
		{
			tokens[i] = token;
       			token = strtok( NULL, "," );
		}

		//tokens[0] can only be a reg
		if (tokens[0] != NULL)
			instr.op1 = reg(tokens[0]);

		//tokens[1] can be a reg, an immed, or both for memory operations
		//check in the following order: reg, both, immed
		if (tokens[1] != NULL)
		{

			if (reg(tokens[1]) >= 0)
			{
				instr.op2 = reg(tokens[1]);
			}
			else
			{
				char* regStart = NULL;

				if (tokens[1][0] == '%') //macro
				{
					regStart = strchr(tokens[1], ')');
					if (regStart == NULL)
					{
						printf("Error: malformed parameter!\n");
						continue;
					}

					regStart++;
					if (regStart[0] != '(') regStart = NULL;
				}
				else //no macro
				{
					regStart = strchr(tokens[1], '(');
				}
				if (regStart == NULL)
				{
					instr.immed = imm(tokens[1]);
				}
				else
				{
					char* regStop = strchr(regStart, ')');
					if (regStop == NULL)
					{
						printf("Error: malformed parameter!\n");
						continue;
					}
					regStart++;
					regStop[0] = '\0';
					instr.op2 = reg(regStart);
					regStart--;
					regStart[0] = '\0';
					instr.immed = imm(tokens[1]);
				}
			}

		}

		//tokens[2] can be a reg or an immed
		if (tokens[2] != NULL)
		{
			if (reg(tokens[2]) < 0)
				instr.immed = imm(tokens[2]);
			else instr.op3 = reg(tokens[2]);
		}

		emulator(&instr);
	}
}

//returns the number associated with the input register
//returns -1 if it fails
int reg(char* reg)
{
	if (!strcmp(reg,"zero")) return 0;

	else if (!strcmp(reg,"at")) return 1;
	else if (!strcmp(reg,"et")) return 24;
	else if (!strcmp(reg,"bt")) return 25;
	else if (!strcmp(reg,"gp")) return 26;
	else if (!strcmp(reg,"sp")) return 27;
	else if (!strcmp(reg,"fp")) return 28;
	else if (!strcmp(reg,"ea")) return 29;
	else if (!strcmp(reg,"ba")) return 30;
	else if (!strcmp(reg,"ra")) return 31;

	else if (strlen(reg) == 2)
	{
		if (reg[0] == 'r' && isdigit(reg[1]))
			return (reg[1] - 48); //ascii conversoin
		else return -1;
	}
	else if (strlen(reg) == 3)
	{
		if (reg[0] == 'r' && isdigit(reg[1]) && isdigit(reg[2]))
			return ((reg[1]-48)*10 + (reg[2]-48)); //ascii conversion
		else return -1;
	}
	else if (strlen(reg) == 4)	//control regs
	{
		if (reg[0] == 'c' && reg[1] == 't' && reg[2] == 'l' && isdigit(reg[3]))
			return (reg[3] - 48);
		else return -1;
	}

	else return -1;
}

//returns numeric value of immidiate value
//returns guarbage for malformed immidiates
//takes maximum 32-bit input
//casts that input in emulator to 16-bit/5-bit values when needed
int imm(char* immed)
{
	// check if the imm is a label before attepting numerical parsing
	unsigned int* lbl_val = label_find(immed);
	if (lbl_val != NULL) return (*lbl_val);

	// first check for the %marcos -> %hi, %lo, %hiadj, %gprel
	if (immed[0] == '%') // macro condition
	{
		//isolate the number by finding brackets
		//use recursion to get the value of the number
		//perform the macro operation

		char* open_bracket = strchr(immed, '(');
		char* close_bracket = strrchr(immed, ')');

		//check for missing brackets
		//instruction will still execute but with an incorrect immidiate value
		if (open_bracket == NULL || close_bracket == NULL)
			return 0;

		//isolate the value between the brackets
		close_bracket[0] = '\0';
		open_bracket++;
		int imm32 = imm(open_bracket);

		if (immed[1] == 'l' && immed[2] == 'o')
			return (imm32 & 0xffff);
		else if (immed[1] == 'h' && immed[2] == 'i' && immed[3] != 'a')
			return ((imm32 >> 16) & 0xffff);
		else if (immed[1] == 'h' && immed[2] == 'i' && immed[3] == 'a')
			return (((imm32 >> 16) & 0xffff) + ((imm32 >> 15) & 0x1));
		else
			return imm32;
	}
	else // no macro
	{
		int i;
		for (i = 0; i < strlen(immed); i++)
		{
			if (immed[i] == 'b')
			{
				immed[i] = '0';
				int i;
				int j = 0;
				int val = 0;
				for (i = strlen(immed)-1; i > -1; i--)
				{
					if (immed[i] == '1')
						val = val + pow(2, j);
					if (immed[i] == '-')
						val = -val;
					j++;
				}
				return val;
			}
			if (immed[i] == 'o' || immed[i] == 'd' || immed[i] == 'x')
			{
				char base [2];
				base[0] = '%';
				base[1] = immed[i];
				immed[i] = '0';	//replace the base letter with a zero
				int val;
				sscanf(immed, base, &val);
				return val;
			}
		}
		//implicit base is decimal
		int val;
		sscanf(immed, "%d", &val);
		return val;
	}
}

void directive(char* directive)
{
	if (strcmp(directive, ".equ"))
	{
		printf("Invalid directive!\n\n");
		return;
	}
	char rawequ [64000];
	fgets(rawequ, 64000, stdin);

	//remove white space here
	int i = 0;
	int j = 0;
	char equ[strlen(rawequ)];
	for (i = 0; i <= strlen(rawequ); i++)
	{
		if (	rawequ[i] != ' ' &&
			rawequ[i] != '\r' &&
			rawequ[i] != '\n' &&
			rawequ[i] != '\t')
		{
			equ[j] = rawequ[i];
			j++;
		}
	}

	char* data = strchr(equ, ',');
	if (data == NULL)
	{
		printf("Error: comma expected!\n");
		return;
	}

	data++;

	int value = imm(data);
	data--;
	data[0] = '\0';

	char* caption = equ;

	printf("# %s = 0x%x\n", caption, value);

	label_add(value, caption);
}

void label_add(unsigned int value, char* caption)
{
	label* index = label_head;

	label* newlabel = (label*) malloc(sizeof(label));
	newlabel->value = value;
	newlabel->caption = caption;
	newlabel->next = NULL;

	if (index == NULL) //empty scenario
	{
		label_head = newlabel;
		return;
	}

	label* last_index;
	while(index != NULL)
	{
		if (!strcmp(index->caption, caption))
		{
			index->value = value;
			return;
		}
		last_index = index;
		index = index->next;
	}
	last_index->next = newlabel;
}

unsigned int* label_find(char* caption)
{
	label* index = label_head;
	while(index != NULL)
		if (!strcmp(index->caption, caption))
			return &(index->value);
	return NULL;
}
