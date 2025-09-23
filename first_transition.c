#include "lib.h"

struct symbol* firstTransition(char *fileName)
/*
	Creates the file for the first transition(it is later deleted).
*/
{
	int DC = 0; /* Data-Counter */
	int IC = 0; /* Instruction-Counter*/

	int numOfWords; /* The number of words occupied by the machine code of a given instruction */

	char *txtFileName;

	FILE* binaryFile; 
	FILE* txtFile;

	struct symbol* symbolTable = NULL;

	char line[MAX_SIZE];

	int isError = 0;
	int numLine = 1;

	int inSymbol = 0;

	int i;

	char *dataBinary = NULL;
	int dataWorked;

	int passedSuccessfully;

	txtFileName = (char *)malloc((strlen(fileName) + strlen(".txt") + 1) * sizeof(char));
	if (txtFileName == NULL) {
        	fprintf(stdout,"Memory reallocation failed\n");
        	exit(1);
    	}

	strcpy(txtFileName, fileName);
	strcat(txtFileName, ".txt");
	
	txtFile = fopen(txtFileName, "r");

	if (!txtFile) {
        	fprintf(stdout, "Error opening file: %s\n", txtFileName);
        	exit(1);
    	}

	binaryFile = fopen(BINARYFILENAMEFIRST,"w");

	if (!binaryFile) {
        	fprintf(stdout, "Error creating file: %s\n", BINARYFILENAMEFIRST);
        	exit(1);
    	}
	/* Reading source file*/
	while(fgets(line,MAX_SIZE,txtFile))
	{
		if(isEmpty(line) || isNotation(line)) /* Skip line*/
		{
			numLine++;
			continue;
		}
		numOfWords = 0;
		inSymbol = 0;
		
		if(strcmp(".define",getNumberWordInStr(line,1)) == 0)
		{
			if(inSymbolTable(getNumberWordInStr(line,2),symbolTable))
			{
				isError = 1;
				fprintf(stdout,"On line %d: constant %s is already defined.\n",numLine,getNumberWordInStr(line,2));
			}
			else
				addSymbol(getNumberWordInStr(line,2),"mdefine" , atoi(getNumberWordInStr(line,4)), &symbolTable);
			numLine++;
			continue;
		}
		if(isSymbol(getNumberWordInStr(line,1)))
		{
			inSymbol = 1;
		}
		if(isDataDef(line)||isStringDef(line))
		{		
			if(inSymbol)
			{
				if(inSymbolTable(getSymbolName(line),symbolTable))
				{
					isError = 1;
					fprintf(stdout,"On line %d: symbol %s is already defined.\n",numLine,getSymbolName(line));
				}
				else
					addSymbol(getSymbolName(line), "data", DC, &symbolTable);
			}

			i = getStartOfDef(line,inSymbol);
 
			if(isDataDef(line))
			{	
				dataWorked = getDataBinary(line,&dataBinary,i,symbolTable,&DC); 
				if(!dataWorked)
					fprintf(binaryFile,"%s",dataBinary);
				else
				{
					isError = 1;
					if(dataWorked == 1)
						fprintf(stdout,"On line %d: constant %s is not defined.\n",numLine,dataBinary);
					if(dataWorked == 2)
						fprintf(stdout,"On line %d: illegal syntax,punctuation or extraneous text after end of instruction.\n",numLine);
				}
				free(dataBinary);	
			}
			else /* isStringDef */
			{
				i++; /* Skip " */
				while(line[i] != '"')
				{
					fprintf(binaryFile,"%s\n",charToBinary(line[i]));
					i++;
					DC++;
					if(line[i] == '\0')
					{
						fprintf(stdout,"On line %d: illegal syntax,punctuation or extraneous text after end of instruction.\n",numLine);
						isError = 1;
						break;
					}
				}
				fprintf(binaryFile,"%s\n",decimalToBinary(0));
				DC++;		
			}
		}
		else if(isExternDef(line) || isEntryDef(line))
		{
			if(isExternDef(line))
			{
				addSymbol(getExternName(line), "ext",0,&symbolTable);
			}
			numLine++;
			continue;
		}
		else /* Instruction line*/
		{
			if(inSymbol)
			{
				if(inSymbolTable(getSymbolName(line),symbolTable))
				{
					isError = 1;
					fprintf(stdout,"On line %d: symbol %s is already defined.\n",numLine,getSymbolName(line));
				}
				else
					addSymbol(getSymbolName(line), "code", IC + 100, &symbolTable);
			}

			if(actionExist(line,inSymbol) == -1)
			{
				fprintf(stdout,"On line %d: there is no such instruction.\n",numLine);
				isError = 1;
			}
			else
			{
				numOfWords = getNumOfWords(line,inSymbol);

				passedSuccessfully = analyzeInstruction(line,&dataBinary,inSymbol);

				if(passedSuccessfully == 1) /* No errors*/
				{
					fprintf(binaryFile,"%s",dataBinary);
					IC += numOfWords;
				}
				else if(passedSuccessfully == 0)
				{
					fprintf(stdout,"On line %d: illegal number of operands for such instruction.\n",numLine);
					isError = 1;
				}
				else if(passedSuccessfully == -1)
				{
					fprintf(stdout,"On line %d: operands are in illegal adressing methods for such instruction.\n",numLine);
					isError = 1;
				}
				else if (passedSuccessfully == -3)
				{
					fprintf(stdout,"On line %d: illegal syntax,punctuation or extraneous text after end of instruction.\n",numLine);
					isError = 1;
				}
				else /* passedSuccessfully == -4*/
				{
					fprintf(stdout,"On line %d: no such addressing method.\n",numLine);
					isError = 1;
				}
				
				free(dataBinary);
			}
		}

		numLine++;
		fprintf(binaryFile,"\n");	
	}

	if(isError) /* Error found*/
	{
		fprintf(stdout,"Program did not go through compilation process.\n");
		remove(BINARYFILENAMEFIRST);
		remove(txtFileName);
		exit(1);
	}

	add100ToDataCode(&symbolTable,IC);

	fclose(binaryFile);
	fclose(txtFile);

	return symbolTable;
}

int analyzeInstruction(char line[],char** dataBinary,int inSymbol) 
/* 
	Make binary code for first word in instruction sentence(rest of words in second transition).
*/
{
	int i = 0;
	int actionNum = actionExist(line,inSymbol);

	int numOfOperands = getNumOfOperands(line,inSymbol);

	int firstOperand = -1, secondOperand = -1;
	int legal;
	
	*dataBinary = (char*)(malloc((WORD_LENGTH + 2) * sizeof(char)));
	if(*dataBinary == NULL)
	{
		fprintf(stdout,"Memory reallocation failed.\n");
		exit(1);
	}
	
	/*First word*/

	/* Empty bits*/
	for(i = 0; i < 4; i++)
		(*dataBinary)[i] = '0';

	/* A.R.E code*/
	(*dataBinary)[13] = '0';
	(*dataBinary)[12] = '0';

	/* opcode*/
	i = 7;
	while(i >= 4)
	{
		(*dataBinary)[i] = (actionNum & 1) + '0'; 
    		actionNum >>= 1; 
    		i--;
	}

	actionNum = actionExist(line,inSymbol);

	/* non-use bits*/

	for(i = 0; i < 4; i++)
		(*dataBinary)[i] = '0';

	if(numOfOperands == -1)
		return -3; /* Illegal syntax*/
	else if(numOfOperands == 0)
	{
		

		for(i = 8; i <= 11; i++)
			(*dataBinary)[i] = '0';
	}
	else if(numOfOperands == 1)
	{
		firstOperand = getFirstOperand(line,inSymbol);
	
		if(firstOperand < -1)
			return firstOperand;
		
		/* No source operand*/
		(*dataBinary)[9] = '0';
		(*dataBinary)[8] = '0';

		if(firstOperand == -1 || firstOperand == 0)
		{
			(*dataBinary)[11] = '0';
			(*dataBinary)[10] = '0';
		}
		else if(firstOperand == 1)
		{
			(*dataBinary)[11] = '1';
			(*dataBinary)[10] = '0';
		}
		else if(firstOperand == 2)
		{
			(*dataBinary)[11] = '0';
			(*dataBinary)[10] = '1';
		}
		else /* firstOperand == 3*/
		{
			(*dataBinary)[11] = '1';
			(*dataBinary)[10] = '1';
		}
	}
	else /* numOfOperands == 2*/
	{
		/* Source operand*/
		firstOperand = getFirstOperand(line,inSymbol);
			
		if(firstOperand < -1)
			return firstOperand;

		if(firstOperand == -1 || firstOperand == 0)
		{
			(*dataBinary)[9] = '0';
			(*dataBinary)[8] = '0';
		}
		else if(firstOperand == 1)
		{
			(*dataBinary)[9] = '1';
			(*dataBinary)[8] = '0';
		}
		else if(firstOperand == 2)
		{
			(*dataBinary)[9] = '0';
			(*dataBinary)[8] = '1';
		}
		else /* firstOperand == 3*/
		{
			(*dataBinary)[9] = '1';
			(*dataBinary)[8] = '1';
		}

		/* Destination operand*/
		secondOperand = getSecondOperand(line,inSymbol);

		if(secondOperand == -1 || secondOperand == 0)
		{
			(*dataBinary)[11] = '0';
			(*dataBinary)[10] = '0';
		}
		else if(secondOperand == 1)
		{
			(*dataBinary)[11] = '1';
			(*dataBinary)[10] = '0';
		}
		else if(secondOperand == 2)
		{
			(*dataBinary)[11] = '0';
			(*dataBinary)[10] = '1';
		}
		else /* secondOperand == 3*/
		{
			(*dataBinary)[11] = '1';
			(*dataBinary)[10] = '1';
		}
	}

	legal = legalInstruction(actionNum,firstOperand,secondOperand,numOfOperands);

	if(legal != 1)
		return legal;

	(*dataBinary)[WORD_LENGTH] = '\n';

	(*dataBinary)[WORD_LENGTH + 1] = '\0';

	return 1; /* Passed succesfully.*/
}

int legalInstruction(int numAction, int firstOperand, int secondOperand, int numOfOperands)
/* 
	Returns 1 if the instruction is legal and different numbers for different errors.
*/
{
	if(numOfOperands == 0)
	{
		if(numAction == 14 || numAction == 15)
			return 1;
		else
			return 0; /* Illegal number of operands*/
	}
	else if(numOfOperands == 1)
	{
		if(numAction == 14 || numAction == 15 || numAction == 6 || (numAction >= 0 && numAction <= 3))
			return 0; /* Illegal number of operands*/
		else if(numAction == 12)
			return 1; /* All operand types*/
		else if(numAction == 13 || numAction == 10 || numAction == 9)
		{
			if(firstOperand == 1 || firstOperand == 3)
				return 1;
			else 
				return -1; /* Illegal operand type*/
		}
		else 
		{
			if(firstOperand != 0)
				return 1;
			else 
				return -1; /* Illegal operand type*/
		}
	}
	else /* numOfOperands == 2*/
	{
		if(numAction == 6)
		{
			if((firstOperand == 1 || firstOperand == 2) && secondOperand != 0)
				return 1;
			else 
				return -1; /* Illegal operand type*/
		}
		else if(numAction == 0 || numAction == 2 || numAction == 3)
		{
			if(secondOperand != 0)
				return 1;
			else
				return -1; /* Illegal operand type*/
		}
		else if(numAction == 1)
			return 1; /* All operands*/
		else 
			return 0; /* Illegal number of operands*/
	}
}

int getDataBinary(char line[],char** dataBinary,int startIndex,struct symbol* symbolTable, int *DC)
/* 
	Gets the binary code of a data definition sentence.
*/
{
	int i = startIndex,j = 0;
	int inComma = 1, inConst = 0, inNum = 0;
	char arg[MAX_SYMBOL_LENGTH+1];
	int numArg = 0;

	struct symbol* constant;

	*dataBinary = (char*)(malloc((WORD_LENGTH+2)*sizeof(char)));
	if(*dataBinary == NULL)
	{
		fprintf(stdout,"Memory reallocation failed\n");
		exit(1);
	}

	while(line[i])
	{
		if(inComma && !inConst && !inNum)/* Start of arg*/
		{
			if(isdigit(line[i]) || line[i] == '+' || line[i] == '-')
			{
				arg[j++] = line[i];
				inNum = 1;
				numArg++;
				(*DC)++;
			}
			if(isalpha(line[i]))
			{
				arg[j++] = line[i];
				inConst = 1;
				numArg++;
				(*DC)++;
			}	
		} 
		else if(inComma && (inConst || inNum))/* In arg*/
		{
			if(inNum)
			{
				if(isdigit(line[i]))
					arg[j++] = line[i];
				else/* End of number arg*/
				{
					if(line[i] != ',')
						inComma = 0;
					arg[j] = '\0';
					j = 0;
					if(numArg == 1)
						strcpy(*dataBinary,decimalToBinary(atoi(arg)));
					else
						strcat(*dataBinary,decimalToBinary(atoi(arg)));
					strcat(*dataBinary,"\n");

					inNum = 0;
					*dataBinary = (char*)(realloc(*dataBinary,(numArg+1)*(WORD_LENGTH+2)*sizeof(char)));
					if(*dataBinary == NULL)
					{
						fprintf(stdout,"Memory reallocation failed\n");
						free(*dataBinary);
						exit(1);
					}
				}
			}
			else /* inConst*/
			{
				if(isalpha(line[i]))
					arg[j++] = line[i];
				else /* End of constant arg*/
				{
					if(line[i] != ',')
						inComma = 0;
					arg[j] = '\0';
					j = 0;
					constant = getSymbol(arg,symbolTable);
					if(constant == NULL)
					{
						*dataBinary = (char*)(realloc(*dataBinary,(MAX_SYMBOL_LENGTH + 1)*sizeof(char)));
						if(*dataBinary == NULL)
						{
							fprintf(stdout,"Memory reallocation failed\n");
							free(*dataBinary);
							exit(1);
						}
						strcpy(*dataBinary,arg);
						return 1;
					}
					else
					{
						if(numArg == 1)
							strcpy(*dataBinary,decimalToBinary(constant->val));
						else
							strcat(*dataBinary,decimalToBinary(constant->val));
						strcat(*dataBinary,"\n");
						inNum = 0;
						*dataBinary = (char*)(realloc(*dataBinary,(numArg+1)*(WORD_LENGTH+2)*sizeof(char)));
						if(*dataBinary == NULL)
						{
							fprintf(stdout,"Memory reallocation failed\n");
							free(*dataBinary);
							exit(1);
						}
					}
				}
			}
		}
		else /* !inComma*/
		{
			if(line[i] == ',')
				inComma = 1;
			else if(!isspace(line[i]))
			{
				return 2;
			}
		}
		i++;
	}

	if(inComma)
		return 2;

	return 0; /* Everything passed succesfully*/
}

void add100ToDataCode(struct symbol **head, int IC)
/*
	Adds 100 and the Instruction Counter to every data definition in the symbol table.
*/
{
	struct symbol *curr = *head;

	while(curr)
	{
		if(strcmp(curr->feature,"data") == 0)
			curr->val = curr->val + 100 + IC;
		curr = curr->next;
	}
}

/* For debugging purposese only */
void printSymbolTable(struct symbol *head)
/*
	Prints the symbol table.
*/
{
	struct symbol *curr = head;
	
	while(curr)
	{
		printf("The name is %s, the feature is %s, the value is %d.\n",curr->name,curr->feature,curr->val);
		curr = curr->next;
	}
}

