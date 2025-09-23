#include "lib.h"

void createOb(char* fileName)
/*
	Creates object file.
*/
{
	int i = 0;
	char binaryCode[MAX_SYMBOL_LENGTH+3];

	int IC = 0,DC = 0,index = 100;

	int inData = 0;
	int inInstruction = 0;

	char* obFileName;
	FILE* obFile;
	
	FILE* binaryFile;
	
	obFileName = (char *)malloc((strlen(fileName) + strlen(".ob") + 1) * sizeof(char));
	if(obFileName == NULL) 
	{
        	fprintf(stdout,"Memory reallocation failed.\n");
        	exit(1);
    	}

	strcpy(obFileName, fileName);
	strcat(obFileName, ".ob");

	obFile = fopen(obFileName,"w");
	if(!obFile) 
	{
    		fprintf(stdout, "Error creating file: %s\n", obFileName);
    		exit(1);
	}

	binaryFile = fopen(BINARYFILENAMESECOND,"r");
	if(!binaryFile) 
	{
    		fprintf(stdout, "Error opening file: %s\n", BINARYFILENAMESECOND);
    		exit(1);
	}

	fprintf(obFile,"          \n");
	while(fgets(binaryCode,MAX_SYMBOL_LENGTH + 3,binaryFile))
	{
		if(isalpha(binaryCode[0]))
		{
			while(binaryCode[i] != '!' && binaryCode[i] != '\0') /* Until end or until sign for extern definition*/
				i++;

			if(binaryCode[i] == '\0') /* Reached end so no extern definition*/
			{
				if(binaryCode[0] == 'D') /* Data line*/
					inData = 1;
				else if(binaryCode[0] == 'I')/* Instruction line*/
					inInstruction = 1;
			}
		}
		else if(isEmpty(binaryCode))
		{
			inData = 0;
			inInstruction = 0;
		}		
		else /* Binary Code*/
		{
			if(inData)
				DC++;
			else if(inInstruction)
				IC++;
			fprintf(obFile,"%04d  ",index);
			index++;
			fprintf(obFile,"%s\n",binaryToEncrypted(binaryCode));
		}
	}
	
	/* Print title*/
	fseek(obFile, 0, SEEK_SET);
	
	fprintf(obFile,"   %d %d",IC,DC);

	fclose(obFile);
	fclose(binaryFile);
}

char* binaryToEncrypted(char* binaryCode)
/*
	Receives binary code and translates to base 4 encrypted code
*/
{
	int i = 0, j = 0;
	char* encrypted = (char*)(malloc((OBJ_LINE_LENGTH+1) * sizeof(char)));
	if(encrypted == NULL)
	{
		fprintf(stdout,"Memory reallocation failed.\n");
		exit(1);
	}

	
    	for (i = 0; i < WORD_LENGTH; i += 2) 
	{
        	if (binaryCode[i] == '0' && binaryCode[i + 1] == '0') 
		{
            		encrypted[j++] = '*';
        	} 
		else if (binaryCode[i] == '0' && binaryCode[i + 1] == '1') 
		{
            		encrypted[j++] = '#';
        	} 
		else if (binaryCode[i] == '1' && binaryCode[i + 1] == '0') 
		{
            		encrypted[j++] = '%';
        	}
 		else
		{
            		encrypted[j++] = '!';
        	} 
    	}

    	encrypted[j] = '\0';

	return encrypted;
}

void createEnt(char* fileName, struct symbol* symbolTable)
/* 
	Creates entry file if needed.
*/
{
	int isEntry = 0;

	struct symbol* curr = symbolTable;

	char* entFileName;
	FILE* entFile;
	
	entFileName = (char *)malloc((strlen(fileName) + strlen(".ent") + 1) * sizeof(char));
	if (entFileName == NULL) {
        	fprintf(stdout,"Memory reallocation failed\n");
        	exit(1);
    	}

	strcpy(entFileName, fileName);
	strcat(entFileName, ".ent");

	entFile = fopen(entFileName,"w");
	if (!entFile) 
	{
    		fprintf(stdout, "Error creating file: %s\n", entFileName);
    		exit(1);
	}

	while(curr)
	{
		if(strcmp(curr->feature,"ent") == 0)
		{
			isEntry = 1;
			fprintf(entFile,"%-6s  %04d\n",curr->name,curr->val);
		}
		curr = curr->next;
	}

	if(!isEntry) /* No entry definitions*/
		remove(entFileName);

	fclose(entFile);
}

void createExt(char* fileName)
/*
	Creates external file if needed.
*/
{
	int index = 100;

	int i;

	int isExternal = 0;

	char* extFileName;
	FILE* extFile;

	FILE* binaryFile;

	char binaryCode[MAX_SYMBOL_LENGTH + 3];
	char symbol[MAX_SYMBOL_LENGTH];
	
	extFileName = (char *)malloc((strlen(fileName) + strlen(".ext") + 1) * sizeof(char));
	if (extFileName == NULL) {
        	fprintf(stdout,"Memory reallocation failed\n");
        	exit(1);
    	}

	strcpy(extFileName, fileName);
	strcat(extFileName, ".ext");

	extFile = fopen(extFileName,"w");
	if (!extFile) 
	{
    		fprintf(stdout, "Error creating file: %s\n", extFileName);
    		exit(1);
	}

	binaryFile = fopen(BINARYFILENAMESECOND,"r");
	if(!binaryFile) 
	{
    		fprintf(stdout, "Error opening file: %s\n", BINARYFILENAMESECOND);
    		exit(1);
	}

	while(fgets(binaryCode,MAX_SYMBOL_LENGTH + 3,binaryFile))
	{
		if(isdigit(binaryCode[0]))
			index++;
		else if(isalpha(binaryCode[0]))
		{
			i = 0;
			while(binaryCode[i])
			{
				if(binaryCode[i] == '!')
				{
					isExternal = 1;
					symbol[i] = '\0';
					fprintf(extFile,"%-6s  %04d\n",symbol,index);
					break;
				}
				else
				{
					symbol[i] = binaryCode[i];
				}
				i++;
			}	
		}
	}

	if(!isExternal) /* No external definitions*/
		remove(extFileName);
	fclose(extFile);
}

