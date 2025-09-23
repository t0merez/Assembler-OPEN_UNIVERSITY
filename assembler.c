#include "lib.h"

int main(int argc, char *argv[])
/*
	Receives files from commanding line containing assembly code and makes assembly code files,object files,entry files and external files accordingly.
*/
{
	int i;
	struct symbol* symbolTable;
	char* txtFileName;
		
	for(i = 1; i < argc; i++)
	{
		createPreAssembly(argv[i]);

		symbolTable = firstTransition(argv[i]);
		secondTransition(argv[i],&symbolTable);

		createOb(argv[i]);
		createEnt(argv[i],symbolTable);
		createExt(argv[i]);
		
		txtFileName = (char *)malloc((strlen(argv[i]) + strlen(".txt") + 1) * sizeof(char));
		if (txtFileName == NULL) {
        		fprintf(stdout,"Memory reallocation failed.\n");
        		exit(1);
    		}

		strcpy(txtFileName, argv[i]);
		strcat(txtFileName, ".txt");

		remove(BINARYFILENAMEFIRST);
		remove(BINARYFILENAMESECOND);
		remove(txtFileName);
		free(txtFileName);

		fprintf(stdout,"Assembler on file: %s is complete.\n",argv[i]);
	}

	return 1;
}
