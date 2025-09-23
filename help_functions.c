#include "lib.h"

int isNotation(char* line)
/*
	Returns 1 if the line is a notation and 0 if otherwise.
*/
{
	int i = 0;
	while(line[i] != '\0')
	{
		if(line[i] == ';')
			return 1;
		i++;
	}
	return 0;
}

char* getExternName(char* line)
/*
	Gets the name of the external definition symbol name.
*/
{
	char* externName;

	int i = 0, j = 0;

	externName = (char*)(malloc((MAX_SIZE+1)*sizeof(char)));
	if(externName == NULL)
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

	while(line[i] != 'n')
		i++;
	i++;
	
	while(isspace(line[i]))
		i++;

	while(isalnum(line[i]))
		externName[j++] = line[i++];

	externName[j] = '\0';

	return externName;
}

int isExtern(char operand[],struct symbol* symbolTable)
/*
	Returns 1 if the operand is external and 0 if not.
*/
{
	struct symbol* curr = symbolTable;
	
	while(curr)
	{
		if(strcmp(operand,curr->name) == 0)
		{
			if(strcmp(curr->feature,"ext") == 0)
				return 1;
			else
				return 0;
		}
		curr = curr->next;
	}	

	return 0;		
}

char* intToBinary(int number, int numBits) 
/*
	Receives a number and returns a string of the binary code in a given number of bits.
*/
{
	int i;
	unsigned int unsignedNumber;
    	char* binary = (char*)malloc(numBits + 1);
    	if (binary == NULL) 
	{
        	fprintf(stdout, "Memory allocation failed.\n");
        	exit(1);
    	}

    	binary[numBits] = '\0'; 

    	unsignedNumber = (unsigned int)number;

    	for (i = numBits - 1; i >= 0; i--) 
	{
        	binary[i] = (unsignedNumber & 1) + '0'; 
        	unsignedNumber >>= 1; 
    	}

    	return binary;
}

int getSymbolVal(char operand[],struct symbol* symbolTable)
/*
	Returns the value of the given symbol name. Returns INT_MIN if the symbol is external and INT_MAX if there is no such symbol in the symbol table.
*/
{
	struct symbol* curr = symbolTable;
	
	while(curr)
	{
		if(strcmp(operand,curr->name) == 0)
		{
			if(strcmp(curr->feature,"ext") == 0)
				return INT_MIN;
			else
				return curr->val;
		}
		curr = curr->next;
	}
	return INT_MAX;
}

int getSecondOperand(char line[],int inSymbol)
/*
	Gets adressing method of the second operand.
*/
{
	int i = 0;

	if(inSymbol)
	{
		while(line[i] != ':')
			i++;
		i++;
	}
	while(isspace(line[i]))
		i++;
	i += 4; /* Skip instruction*/
	
	while(line[i] != ',')
		i++;
	i++;

	while(isspace(line[i]))
		i++;

	if(line[i] == '#')
		return 0; /* Immediate referral method*/
	if(line[i] == 'r' && isdigit(line[i+1]) && (isspace(line[i+2]) || line[i+2] == ',')) /* Direct register address method*/
		return 3;
	if(isalpha(line[i]))
	{
		while(isalpha(line[i]))
		{
			i++;
		}
		if(line[i] == '[')
				return 2; /* Fixed index address method*/
		return 1; /* Direct adress method*/
	}
	return -4; /* No such adress method*/

}

int getFirstOperand(char line[],int inSymbol)
/*
	Gets adressing method of the first operand.
*/
{
	int i = 0;
	
	if(inSymbol)
	{
		while(line[i] != ':')
			i++;
		i++; /* Skip ':'*/
	}

	while(isspace(line[i]))
		i++;
	i += 4; /* Skip instruction*/
	
	while(isspace(line[i]))
		i++;

	if(line[i] == '#')
		return 0; /* Immediate referral method*/
	if(line[i] == 'r' && isdigit(line[i+1]) && (isspace(line[i+2]) || line[i+2] == ',')) /* Direct register address method*/
		return 3;
	if(isalpha(line[i]))
	{
		while(isalpha(line[i]))
		{
			i++;
		}
		if(line[i] == '[')
				return 2; /* Fixed index address method*/
		return 1; /* Direct adress method*/
	}
	return -4; /* No such adress method*/
}

int getNumOfOperands(char line[],int inSymbol)
/*
	Returns the number of operands in line and checks for illegal syntax.
*/
{
	int i = 0;
	int isSpace = 0;

	if(inSymbol)
	{
		while(line[i] != ':')
			i++;
		i++; /* Skip ':'*/
	}

	while(isspace(line[i]))
		i++;
	i += 4; /* Skip instruction*/

	while(isspace(line[i]))
		i++;

	if(line[i] == '\0')
		return 0; /* no operands*/

	while(line[i] != ',')
	{
		if(line[i] == '\0')
			return 1;
		if(isspace(line[i]))
			isSpace = 1;
		if(isSpace && !isspace(line[i]))
			return -1;
		i++;
	}
	i++;
	
	while(isspace(line[i]))
		i++;
	if(line[i] == '\0')
		return -1;
	
	while(!isspace(line[i]))
	{
		if(line[i] == ',')
			return -1;
		i++;
	}
	
	while(isspace(line[i]))
		i++;

	if(line[i] == '\0')
		return 2;
	else
		return -1;	
}

int getNumOfWords(char line[], int inSymbol) 
/*
	Gets the number of words needed for instruction line
*/
{
	int i = 0;
    	int numOfWords = 1;
    	int isRegister = 0;

    	if (inSymbol) {
        	while (line[i] != ':')
            		i++;
        	i++; /* Skip ':'*/
    	}

    	while (isspace(line[i]))
        	i++;

    	i += 4; /* Skip action(3 letters) then space after */

    	while (isspace(line[i]) && line[i] != '\0' && line[i] != '\n')
        	i++;
    	if (line[i] == '\0' || line[i] == '\n')
        	return numOfWords;
    	/*First operand*/
    	if (isalpha(line[i])) 
	{
        	if ((line[i] == 'r' && isdigit(line[i + 1])) && (isspace(line[i + 2]) || line[i + 2] == ',' || line[i + 2] == '\0' || line[i + 2] == '\n'))
            		isRegister = 1;
        	numOfWords++;
        	while (isalnum(line[i]))
            		i++;
        	if (line[i] == '[')
            		numOfWords++;

    	} 
	else if (line[i] == '#')
        	numOfWords++;

    	while (!isspace(line[i]) && line[i] != ',' && line[i] != '\0' && line[i] != '\n')
        	i++;
    	if (line[i] == '\0' || line[i] == '\n')
        	return numOfWords;

    	if (isspace(line[i])) 
	{
        	while (line[i] != ',') 
		{
            		if (line[i] == '\0' || line[i] == '\n')
                		return numOfWords;
            		if (!isspace(line[i]))
                		return -3; /* unnecessary punctuation or missing punctuation.*/
            		i++;
        	}
    	}
    	i++; /* Skip comma */

    	while (isspace(line[i]) && line[i] != '\0' && line[i] != '\n')
        	i++;
    	if (line[i] == '\0' || line[i] == '\n')
        	return -3; /* unnecessary punctuation or missing punctuation.*/
    
    	/* Second operand*/
   	if (isalpha(line[i])) 
    	{
        	if (line[i] == 'r' && isdigit(line[i + 1]) && (isspace(line[i + 2]) || line[i + 2] == ',' || line[i + 2] == '\0' || line[i + 2] == '\n'))
        	{
            		if (!isRegister)
                		numOfWords++;
        	}
        	else
        	{
            		numOfWords++;  
            		while (isalnum(line[i]))
                		i++;
            		if (line[i] == '[')
                		numOfWords++;  
        	}
    	}
    	else if (line[i] == '#')
        	numOfWords++;
    
    	/* Check for no extra*/
    	while (!isspace(line[i]) && line[i] != ',' && line[i] != '\0' && line[i] != '\n')
        	i++;

    	if (line[i] == ',')
        	return -3; /* unnecessary punctuation or missing punctuation.*/
    	else
        	return numOfWords;
}

int actionExist(char line[],int inSymbol)
/*
	Checks if the instruction in the line exists.
*/
{
	char actions[NUM_OF_ACTIONS][ACTION_LENGTH] = {
	"mov","cmp","add","sub","not","clr","lea","inc",
	"dec","jmp","bne","red","prn","jsr","rts","hlt"};

	char newAction[ACTION_LENGTH];
	
	int i = 0;
	int j = 0;

	if(inSymbol)
	{
		while(line[i] != ':')
			i++;
		i++; /* Skip ':'*/
	}

	while(line[i] == ' ' || line[i] == '\t')
		i++;
	while(j < ACTION_LENGTH-1)
		newAction[j++] = line[i++];
	if(!(isspace(line[i])))
		return -1;
	newAction[j] = '\0'; /* Null terminator*/
	
	for(i = 0;i<NUM_OF_ACTIONS;i++)
	{
		if(strcmp(newAction,actions[i]) == 0)
			return i;
	}
	return -1; /* Action name in line doesn't exist*/
}


char* getSymbolExtern(char line[])
/*
	Receives a line and returns a string of the name of the extern definition.
*/
{
	int i = 0, j = 0;
	char *name;
	
	name = (char*)(malloc((MAX_SYMBOL_LENGTH+1)*sizeof(char)));
	if(name == NULL)
	{
		fprintf(stdout,"Memory allocation failed.\n");
		exit(1);
	}

	while(line[i] == ' ' || line[i] == '\t')
		i++;
	i++; /* Skip '.'*/
	while(isalnum(line[i]))
		i++;
	while(line[i] == ' ' || line[i] == '\t')
		i++;
	while(isalnum(line[i]))
		name[j++] = line[i++];
	name[j] = '\0';
	return name;
}

char* charToBinary(char ch)
/*
	Receives a character and returns a string of the character in 14 bit form.
*/
{
	int i;
	char* binary = (char*)malloc((WORD_LENGTH+1) * sizeof(char)); 
    	if (binary == NULL) 
	{
        	fprintf(stdout, "Memory allocation failed\n");
        	exit(1);
    	}

    	for (i = 0; i < WORD_LENGTH; i++) 
        	binary[i] = (ch & (1 << (WORD_LENGTH - 1 - i))) ? '1' : '0';
    	
    	binary[WORD_LENGTH] = '\0'; 
    	return binary;
}

char* decimalToBinary(int n) 
/*
	Receives a number and returns a string of the number in 14 bit form.
*/
{
	char *bin;
	int i = WORD_LENGTH - 1;
	int carry = 1;
	int j;

	bin = (char*)(malloc((WORD_LENGTH + 1) * sizeof(char))); 
	if (bin == NULL) 
	{
		fprintf(stdout, "Memory allocation failed\n");
		exit(1);
	}

	if(n > 8191 || n < -8192)
    		return NULL;

	    
	for (j = 0; j < WORD_LENGTH; j++) 
	{
		bin[j] = '0';
	}

	if (n >= 0) 
	{
		while (i >= 0) 
		{
		    	bin[i] = (n % BASE) + '0';
		    	n /= BASE;
		    	i--;
		}
	} 
	else /* Negative number */
	{
		n = -n;
		while (i >= 0 && n > 0) 
		{
		    	bin[i] = (n % BASE) + '0';
		    	n /= BASE;
		    	i--;
		}

		
		for (j = 0; j < WORD_LENGTH; j++) /* Flip each bit */
		{
		    	bin[j] = (bin[j] == '0') ? '1' : '0'; 
		}

		/* Add 1 */
		carry = 1;
		i = WORD_LENGTH - 1;
		while (i >= 0 && carry == 1) 
		{
		    	if (bin[i] == '0') 
			{
		        	bin[i] = '1';
		        	carry = 0; 
		    	} 
			else 
		        	bin[i] = '0';
		    	i--;
		}
	}

	bin[WORD_LENGTH] = '\0'; /* Null terminator */

	return bin;
}


int getStartOfDef(char str[], int isSymbol)
/*
	Receives a string and gets the start of a string or data definition.
*/
{
	int i = 0;
	while(str[i] == ' ' || str[i] == '\t')
		i++;
	if(isSymbol)
	{
		while(str[i] != ':')
			i++;
		i++;/* Skip ':'*/
	}
	while(str[i] == ' ' || str[i] == '\t')
		i++;
	while(str[i] != ' ' && str[i] != '\t')
		i++;
	while(str[i] == ' ' || str[i] == '\t')
		i++;
	return i;
}

char* getSymbolName(char str[])
/*
	Receives a line with a symbol definition and returns a string of the symbol name.
*/
{
	int i = 0, j = 0;
	char *symbol;

	while(str[i] == ' ' || str[i] == '\t')
		i++;

	symbol = (char*)(malloc((MAX_SYMBOL_LENGTH+1) * sizeof(char)));
	if(symbol == NULL)
	{
		fprintf(stdout,"Memory allocation failed\n");
		exit(1);
	}
	
	while(str[i] != ':' && j < MAX_SYMBOL_LENGTH)
		symbol[j++] = str[i++];
	symbol[j] = '\0';

	return symbol;
}

int isEntryDef(char str[])
/*
	Returns 1 if there is an entry definition otherwise 0.
*/
{
	int i = 0, j = 0;

	char word[7]; /* Length of ".entry" */
	
	while(str[i] == ' ' || str[i] == '\t')
	    i++;
	    
	if(str[i] == '.') /* No symbol */
	{
		return !(strcmp(getNumberWordInStr(str,1),".entry"));
	}

	/* Skip symbol */ 
	while(str[i] && str[i] != ':')
	{
		i++;
	}

	i++; /* Skip ':' */

	while(str[i] && (str[i] == ' ' || str[i] == '\t'))
	{
		i++;
	}
	
	while(str[i] && (str[i] != ' ' && str[i] != '\t') && j < 7)
	{
		word[j++] = str[i++];
	}
	word[j] = '\0';
	
	return !(strcmp(word,".entry")); 
	
}

int isExternDef(char str[])
/*
	Returns 1 if there is an extern definition otherwise 0.
*/
{
	int i = 0, j = 0;

	char word[8]; /* Length of ".extern" */
	
	while(str[i] == ' ' || str[i] == '\t')
	    i++;
	    
	if(str[i] == '.') /* No symbol */
	{
		return !(strcmp(getNumberWordInStr(str,1),".extern"));
	}

	/* Skip symbol */
	while(str[i] && str[i] != ':')
	{
		i++;
	}

	i++; /* Skip ':' */

	while(str[i] && (str[i] == ' ' || str[i] == '\t'))
	{
		i++;
	}
	
	while(str[i] && (str[i] != ' ' && str[i] != '\t') && j < 7)
	{
		word[j++] = str[i++];
	}
	word[j] = '\0';
	
	return !(strcmp(word,".extern")); 
	
}

int isStringDef(char str[])
/*
	Returns 1 if there is a string definition otherwise 0.
*/
{
	int i = 0, j = 0;

	char word[8]; /* Length of ".string" */
	
	while(str[i] == ' ' || str[i] == '\t')
	    i++;
	    
	if(str[i] == '.') /* No symbol */
	{
		return !(strcmp(getNumberWordInStr(str,1),".string"));
	}

	/* Skip symbol */
	while(str[i] && str[i] != ':')
	{
		i++;
	}

	i++; /* Skip ':' */

	while(str[i] && (str[i] == ' ' || str[i] == '\t'))
	{
		i++;
	}
	
	while(str[i] && (str[i] != ' ' && str[i] != '\t') && j < 7)
	{
		word[j++] = str[i++];
	}
	word[j] = '\0';
	

	return !(strcmp(word,".string")); 
	
}

int isDataDef(char str[])
/*
	Returns 1 if there is a data definition otherwise 0.
*/
{
	int i = 0, j = 0;

	char word[6]; /* Length of ".data" */
	
	while(str[i] == ' ' || str[i] == '\t')
	    i++;
	    
	if(str[i] == '.') /* No symbol */
	{
		return !(strcmp(getNumberWordInStr(str,1),".data"));
	}

	/* Skip symbol */
	while(str[i] && str[i] != ':')
	{
		i++;
	}

	i++; /* Skip ':' */

	while(str[i] && (str[i] == ' ' || str[i] == '\t'))
	{
		i++;
	}
	
	while(str[i] && (str[i] != ' ' && str[i] != '\t') && j < 5)
	{
		word[j++] = str[i++];
	}
	word[j] = '\0';
	

	return !(strcmp(word,".data")); 
}

int isSymbol(char *str)
/*
	Returns 1 if str contains a legal symbol defintion otherwise 0.
*/
{

	int i = 0;

    	if (*str == '\0') 
        	return 0;

	while(isspace(str[i]))
		i++;
    
    	if (!isalpha(str[i]))
        	return 0;  	
    
    	while (str[i] != ':' && str[i] != '\0') 
	{
        	if (!isalnum(str[i])) 
            		return 0;
        
        	i++;
    	}

    	if (str[i] == ':') 
        	return 1;
    	else 
        	return 0;
    
}

char* getNumberWordInStr(char str[],int num)
/*
	Receives a string and a number and returns a string of the number word in the given string.
*/
{
	char* word;
	int i = 0,j = 0;
	int wordNum = 1;
	int inWord = 0;

	word = (char*)(malloc(MAX_SIZE * sizeof(char)));
	if(word == NULL)
	{
		fprintf(stdout,"Memory reallocation failed\n");
		exit(1);
	}

	while(str[i] && wordNum < num)
	{
		if(!inWord && isCodeDigit(str[i]))
		{
			inWord = 1;
		}
		else if(inWord && !isCodeDigit(str[i]))
		{
			inWord = 0;
			wordNum++;
		}
		i++;
	}
	while(!isCodeDigit(str[i]))
		i++;
	inWord = 1;
	while(inWord && str[i])
	{
		if(isCodeDigit(str[i]))
		{
			word[j++] = str[i];
			i++;
		}
		else
			break;
	}
	word[j] = '\0';
	
	return word;
}

int isCodeDigit(char ch)
/*	
	Returns if ch is a digit used in code.
*/
{
	return ch != ' ' && ch != '\t';
}

void removeLeadingSpacesTabs(char *str)
/*
	Removes all the spaces at the start of a string.
*/ 
{
    	int len = strlen(str);
    	int start = 0;

    
    	while (str[start] == ' ' || str[start] == '\t') {
        	start++;
    	}

    
    	memmove(str, str + start, len - start + 1);
}

char* my_strdup(char *src)
/*
	My version of strdup.
*/ 
{
       
    	int i;

    	int length = strlen(src) + 1; 
    
    	char *duplicate = malloc(length);

    	if (duplicate == NULL) 
        	return NULL;
    	if (src == NULL)
        	return NULL;
    	for (i = 0; src[i] != '\0'; i++) {
        	duplicate[i] = src[i];
    	}
    	duplicate[i] = '\0'; 
    
    	return duplicate;
}

int isEmpty(char str[])
/*
	Checks if the line is empty(only contains spaces).
*/
{
	int i;
    	for (i = 0; str[i] != '\0'; i++) {
        	if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r') {
            		return 0; 
        	}
    	}
    	return 1; 
}
