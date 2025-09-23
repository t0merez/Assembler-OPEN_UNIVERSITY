#include "lib.h"

void secondTransition(char *fileName, struct symbol** symbolTable)
/*
	Creates the second transition file(it is deleted later).
*/
{

	char line[MAX_SIZE];

	char binaryCode[WORD_LENGTH+2];

	int isError = 0;
	int passedSuccessfully;

	char *txtFileName;

	FILE* binaryFileFirst; 
	FILE* binaryFileSecond; 
	FILE* txtFile;

	int numLine = 1;

	txtFileName = (char *)malloc((strlen(fileName) + strlen(".txt") + 1) * sizeof(char));
	if (txtFileName == NULL) 
	{
        	fprintf(stdout,"Memory allocation failed here.\n");
        	exit(1);
    	}

	strcpy(txtFileName, fileName);
	strcat(txtFileName, ".txt");
	
	txtFile = fopen(txtFileName, "r");

	if (!txtFile) 
	{
        	fprintf(stdout, "Error opening file: %s.\n", txtFileName);
        	exit(1);
    	}

	binaryFileFirst = fopen(BINARYFILENAMEFIRST,"r");
	if (!binaryFileFirst) 
	{
        	fprintf(stdout, "Error creating file: %s.\n", BINARYFILENAMEFIRST);
        	exit(1);
    	}

	binaryFileSecond = fopen(BINARYFILENAMESECOND,"w");
	if (!binaryFileSecond) 
	{
        	fprintf(stdout, "Error creating file: %s.\n", BINARYFILENAMESECOND);
        	exit(1);
    	}


	while(fgets(line,MAX_SIZE,txtFile))
	{
		if(isStringDef(line) || isDataDef(line))
		{
			fprintf(binaryFileSecond,"D\n"); /* Print used in object file creation */
			while(strcmp(fgets(binaryCode,WORD_LENGTH + 2,binaryFileFirst),"\n") != 0)
				fprintf(binaryFileSecond,"%s",binaryCode);
			fprintf(binaryFileSecond,"\n");
		}
		else if(isExternDef(line) || strcmp(".define",getNumberWordInStr(line,1)) == 0 || isEmpty(line) || isNotation(line))
		{
			numLine++;
			continue;
		}
		else if(isEntryDef(line))
		{
			passedSuccessfully = putEntryDef(line,symbolTable);
			if(passedSuccessfully == 0)
			{
				fprintf(stdout,"On line %d:there is no such symbol defined in file for entry instruction.\n",numLine);
				isError = 1;
			}
			else if(passedSuccessfully == -1)
			{
				fprintf(stdout,"On line %d:it is illegal to define a symbol as an entry and external.\n",numLine);
				isError = 1;
			}
		}
		else /* Instruction line*/
		{
			fprintf(binaryFileSecond,"I\n"); /* Print used in object file creation */
			fgets(binaryCode,WORD_LENGTH + 2,binaryFileFirst);
			fprintf(binaryFileSecond,"%s",binaryCode);
			passedSuccessfully = analyzeOperands(line,*symbolTable,binaryFileSecond);
			if(passedSuccessfully == 0)
			{
				fprintf(stdout,"On line %d: undefined symbol or constant.\n",numLine);
				isError = 1;
			}
			else if(passedSuccessfully == -1)
			{
				fprintf(stdout,"On line %d: no such register.\n",numLine);
				isError = 1;
			}
			else if(passedSuccessfully == -2)
			{
				fprintf(stdout,"On line %d: illegal syntax,punctuation or extraneous text after end of instruction.\n",numLine);
				isError = 1;
			}
			fgets(binaryCode,WORD_LENGTH + 2,binaryFileFirst);
			fprintf(binaryFileSecond,"%s",binaryCode);
		}
		numLine++;
	}

	
	if(isError) /* Error found*/
	{
		fprintf(stdout,"Program did not go through compilation process.\n");
		remove(BINARYFILENAMEFIRST);
		remove(BINARYFILENAMESECOND);
		remove(txtFileName);
		exit(1);
	}

	fclose(binaryFileFirst);
	fclose(binaryFileSecond);
	fclose(txtFile);
}

int analyzeOperands(char line[],struct symbol* symbolTable,FILE* binaryFile)
/*
	Analyzes operands in instruction line and prints the needed binary code to binaryFile.
*/
{
	int value;
	char operand[MAX_SYMBOL_LENGTH];
	int i = 0, j = 0;

	int inRegister = 0;

	int inSymbol = isSymbol(line);
	int numOfOperands = getNumOfOperands(line,inSymbol);

	if(numOfOperands == 0)
		return 1;

	if(inSymbol)
	{
		while(line[i] != ':')
			i++;
		i++;
	}

	while(isspace(line[i]))
		i++;

	i += 4; /* Skip instruction*/

	while(isspace(line[i]))
		i++;

	if(numOfOperands == 1)
	{
		if(line[i] == '#')
		{
			i++;
			while(!isspace(line[i]) && line[i] != '\0')
				operand[j++] = line[i++];
			operand[j] = '\0';

			if(isalpha(operand[0]))
			{
				value = getSymbolVal(operand,symbolTable);
				if(value == INT_MAX)
				{
					return 0; /* No such constant.*/
				}
			}
			else
				value = atoi(operand);
			
			fprintf(binaryFile,"%s",intToBinary(value,12));
			fprintf(binaryFile,"00\n");
			
		}
		else if(line[i] == 'r' && isdigit(line[i+1]) && isspace(line[i+2]))
		{
			if(line[i+1] > '7')
				return -1; /* No such register*/
			fprintf(binaryFile,"000000000");
			fprintf(binaryFile,"%s",intToBinary(line[i+1] - '0',3));
			fprintf(binaryFile,"00\n");
		}
		else
		{
			while(!isspace(line[i]) && line[i] != '[' && line[i] != '\0')
				operand[j++] = line[i++];
			operand[j] = '\0';
			value = getSymbolVal(operand,symbolTable);
			if(value == INT_MAX)
			{
				return 0; /* No such symbol.*/
			}
			if(value == INT_MIN)
			{
				value = 0;
				fprintf(binaryFile,"%s!\n",operand); /* Print that is used in external file creation*/
			}
			fprintf(binaryFile,"%s",intToBinary(value,12));

			if(isExtern(operand,symbolTable))
				fprintf(binaryFile,"01\n");
			else
				fprintf(binaryFile,"10\n");

			if(line[i] == '[')
			{
				i++;
				j = 0;
				while(line[i] != ']')
				{
					if(line[i] == '\0')
						return -2; /* Illegal syntax*/ 
					operand[j++] = line[i++];
				}
				operand[j] = '\0';
		
				if(isalpha(operand[0]))
				{
					value = getSymbolVal(operand,symbolTable);
					if(value == INT_MAX)
						return 0; /* No such constant.*/
				}
				else
					value = atoi(operand);
			
				fprintf(binaryFile,"%s",intToBinary(value,12));
				fprintf(binaryFile,"00\n");
			}
			
		}
	}
	else if(numOfOperands == 2) /* Two operands*/
	{
		if(line[i] == '#')
		{
			i++;
			while(!isspace(line[i]) && line[i] != ',' && line[i] != '\0')
				operand[j++] = line[i++];
			operand[j] = '\0';

			if(isalpha(operand[0]))
			{
				value = getSymbolVal(operand,symbolTable);
				if(value == INT_MAX)
				{
					return 0; /* No such constant.*/
				}
			}
			else
				value = atoi(operand);
			
			fprintf(binaryFile,"%s",intToBinary(value,12));
			fprintf(binaryFile,"00\n");
			
		}
		else if(line[i] == 'r' && isdigit(line[i+1]) && ((isspace(line[i+2]) || line[i+2] == ',')))
		{
			if(line[i+1] > '7')
				return -1; /* No such register*/
			value = line[i+1] - '0';
			inRegister = 1;
		}
		else
		{
			while(!isspace(line[i]) && line[i] != ',' && line[i] != '[' && line[i] != '\0')
				operand[j++] = line[i++];
			operand[j] = '\0';

			value = getSymbolVal(operand,symbolTable);
			if(value == INT_MAX)
			{
				return 0; /* No such symbol.*/
			}
			if(value == INT_MIN) 
			{
				value = 0;
				fprintf(binaryFile,"%s!\n",operand); /* Print that is used in external file creation*/
			}
			fprintf(binaryFile,"%s",intToBinary(value,12));
			if(isExtern(operand,symbolTable))
				fprintf(binaryFile,"01\n");
			else
				fprintf(binaryFile,"10\n");

			if(line[i] == '[')
			{
				i++;
				j = 0;
				while(line[i] != ']')
				{
					if(line[i] == '\0')
						return -2; /* Illegal syntax*/ 
					operand[j++] = line[i++];
				}
				operand[j] = '\0';
		
				if(isalpha(operand[0]))
				{
					value = getSymbolVal(operand,symbolTable);
					if(value == INT_MAX)
						return 0; /* No such constant.*/
				}
				else
					value = atoi(operand);
			
				fprintf(binaryFile,"%s",intToBinary(value,12));
				fprintf(binaryFile,"00\n");
			}
			
		}

		/* Second operand*/

		j = 0;
		while(line[i] != ',')
			i++;
		i++; /* Skip ','*/

		while(isspace(line[i]))
			i++;

		if(inRegister)
		{
			if(line[i] == 'r' && isdigit(line[i+1]) && isspace(line[i+2]))
			{
				if(line[i+1] > '7')
					return -1;
				fprintf(binaryFile,"000000");

				fprintf(binaryFile,"%s",intToBinary(value,3));
				fprintf(binaryFile,"%s",intToBinary(line[i+1] - '0',3));

				fprintf(binaryFile,"00\n");

				return 1;
			}
			else
			{
				fprintf(binaryFile,"000000");
				fprintf(binaryFile,"%s",intToBinary(value,3));
				fprintf(binaryFile,"00000\n");
			}
		}


		if(line[i] == '#')
		{
			i++;
			while(!isspace(line[i]) && line[i] != '\0')
				operand[j++] = line[i++];
			operand[j] = '\0';

			if(isalpha(operand[0]))
			{
				value = getSymbolVal(operand,symbolTable);
				if(value == INT_MAX)
				{
					return 0; /* No such constant.*/
				}
			}
			else
				value = atoi(operand);
			
			fprintf(binaryFile,"%s",intToBinary(value,12));
			fprintf(binaryFile,"00\n");
			
		}
		else
		{
			j = 0;
			while(!isspace(line[i]) && line[i] != '[' && line[i] != '\0')
				operand[j++] = line[i++];
			operand[j] = '\0';

			value = getSymbolVal(operand,symbolTable);
			if(value == INT_MAX)
			{
				return 0; /* No such symbol.*/
			}
			if(value == INT_MIN)
			{
				value = 0;
				fprintf(binaryFile,"%s!\n",operand); /* Print that is used in external file creation*/
			}
			fprintf(binaryFile,"%s",intToBinary(value,12));
			if(isExtern(operand,symbolTable))
				fprintf(binaryFile,"01\n");
			else
				fprintf(binaryFile,"10\n");

			if(line[i] == '[')
			{
				i++;
				j = 0;
				while(line[i] != ']')
				{
					if(line[i] == '\0')
						return -2; /* Illegal syntax*/ 
					operand[j++] = line[i++];
				}
				operand[j] = '\0';
		
				if(isalpha(operand[0]))
				{
					value = getSymbolVal(operand,symbolTable);
					if(value == INT_MAX)
						return 0; /* No such constant.*/
				}
				else
					value = atoi(operand);
			
				fprintf(binaryFile,"%s",intToBinary(value,12));
				fprintf(binaryFile,"00\n");
			}	
		}
	}

	return 1;
}

int putEntryDef(char line[],struct symbol** symbolTable)
/* 
	Changes the defined symbol name in the entry definition to entry in the symbol table.
*/
{
	struct symbol* curr = *symbolTable;

	char* entryName;

	int i = 0, j = 0;

	entryName = (char*)(malloc((MAX_SIZE+1)*sizeof(char)));
	if(entryName == NULL)
	{
		fprintf(stdout,"Memory reallocation failed.\n");
		exit(1);
	}

	if(isSymbol(line))
	{
		while(line[i] != ':')
			i++;
		i++;
	}

	while(line[i] != 'y')
		i++;
	i++;
	
	while(isspace(line[i]))
		i++;

	while(isalnum(line[i]))
		entryName[j++] = line[i++];

	entryName[j] = '\0';
	
	while(curr)
	{
		if(strcmp(entryName,curr->name) == 0)
		{
			if(strcmp(curr->feature,"ext") == 0)
				return -1;
			strcpy(curr->feature,"ent");
			return 1;
		}
		curr = curr->next;
	}

	return 0; /* No symbol found*/
	
}
