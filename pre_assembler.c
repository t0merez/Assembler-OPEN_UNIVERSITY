#include "lib.h"

void createPreAssembly(char* fileName)
/*
	Creates the first assembler file after opening the macros.
*/
{
	FILE *newFile;
	FILE *asFile;
	char *newFileName;
	char *name;
	char *code = NULL;
	char *copyCode;
	char line[MAX_SIZE];
	struct macro* head = NULL;
	struct macro* curr;
	int isMacro = 0;

	char *asFileName;

	int numOfLines = 0;

	FILE* file = fopen(fileName,"r");
	
	if (!file) {
        	fprintf(stdout, "Error opening file: %s\n", fileName);
        	exit(1);
    	}


	newFileName = (char *)malloc((strlen(fileName) + strlen(".txt") + 1) * sizeof(char));
	if (newFileName == NULL) {
        	fprintf(stdout,"Memory reallocation failed\n");
        	exit(1);
    	}

	strcpy(newFileName, fileName);
	strcat(newFileName, ".txt");

	newFile = fopen(newFileName,"w+");

	if (!newFile) {
        	fprintf(stdout, "Error creating file: %s\n", newFileName);
        	exit(1);
    	}

	name = (char*)(malloc(MAX_SIZE * sizeof(char)));
	if (name == NULL) {
        	fprintf(stdout,"Memory reallocation failed\n");
        	exit(1);
    	} 
	
	while(fgets(line,MAX_SIZE,file))
	{
		
		if(isEmpty(line))
		{
			fprintf(newFile,"\n");
			continue;
		}
		curr = inMacroList(getNumberWordInStr(line,1),head); /* Checks if line is calling for a macro*/
		if(curr!=NULL) /* If there is a macro print the code of the macro*/
		{
			fprintf(newFile,"%s",curr->code);
		}
		else if(strcmp(getNumberWordInStr(line,1),"mcr") == 0) /*Start of macro definition*/
		{
			isMacro = 1;
			strcpy(name,getNumberWordInStr(line,2));
		}
		else if(isMacro) 
		{
			if(strcmp(getNumberWordInStr(line,1),"endmcr\n") != 0) /* In the middle of macro*/
			{
				removeLeadingSpacesTabs(line);
				if(numOfLines == 0)
				{
					code = my_strdup(line);
				}
				else
				{
					copyCode = my_strdup(code);

					code = (char *)realloc(code, MAX_SIZE * (numOfLines+1));
					if(code == NULL)
					{
						fprintf(stdout,"Memory reallocation failed\n");
        					exit(1);
					}

					strcat(code,line);
					free(copyCode);
				}
				numOfLines++;		
			
			}
			else /* End of macro definition*/
			{
				isMacro = 0;
				numOfLines = 0;
				addMacro(name,code,&head);
				free(name);
				free(code);
			}
		}
		else /* Not definiton of macro or calling for macro*/
		{
			fprintf(newFile,"%s",line);		
		}
	}

	asFileName = (char *)malloc((strlen(fileName) + strlen(".as") + 1) * sizeof(char));
	if (asFileName == NULL) {
        	fprintf(stdout,"Memory reallocation failed\n");
        	exit(1);
    	}

	strcpy(asFileName, fileName);
	strcat(asFileName, ".as");

	asFile = fopen(asFileName,"w");
	if (!asFile) {
    		fprintf(stdout, "Error creating file: %s\n", asFileName);
    		exit(1);
	}

	fseek(newFile, 0, SEEK_SET);

	while(fgets(line,MAX_SIZE,newFile))
		fprintf(asFile,"%s",line);
	

	fclose(file);
	fclose(newFile);
	fclose(asFile);


}
