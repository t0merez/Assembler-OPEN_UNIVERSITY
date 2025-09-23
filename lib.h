
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <float.h>

#define MAX_SIZE 80
#define WORD_LENGTH 14
#define MAX_SYMBOL_LENGTH 31
#define BASE 2
#define ACTION_LENGTH 4
#define NUM_OF_ACTIONS 16
#define OBJ_LINE_LENGTH 7

#define BINARYFILENAMEFIRST "binaryFirst.txt"
#define BINARYFILENAMESECOND "binarySecond.txt"

struct macro { /* Macro table*/
    	char *name;
    	char *code;
    	struct macro *next;
};

struct symbol { /* Symbol Table*/
    	char *name;
	char *feature;
	int val;
    	struct symbol *next;
};

/* pre_assembler.c */
void createPreAssembly(char*);

/* help_functions.c */
char* getNumberWordInStr(char[],int);
int isCodeDigit(char);
void removeLeadingSpacesTabs(char*);
char *my_strdup(char*);
int isEmpty(char[]);
int isSymbol(char*);
int isDataDef(char[]);
int isStringDef(char[]);
char* getSymbolName(char[]);
int getStartOfDef(char[],int);
char* decimalToBinary(int);
char* charToBinary(char);
int isExternDef(char[]);
int isEntryDef(char[]);
char* getSymbolExtern(char[]);
int actionExist(char[],int);
int getNumOfWords(char[],int);
int getNumOfOperands(char[],int);
int getFirstOperand(char[],int);
int getSecondOperand(char[],int);
int getSymbolVal(char[],struct symbol*);
char* intToBinary(int,int);
int isExtern(char[],struct symbol*);
char* getExternName(char*);
int isNotation(char*);

/* data_stucture_func.c*/
struct macro* inMacroList(char*,struct macro*);
void addMacro(char*,char*,struct macro**);
void addSymbol(char*, char*, int, struct symbol**);
struct symbol* getSymbol(char[],struct symbol*);
int inSymbolTable(char*, struct symbol*);

/* first_transition.c*/
struct symbol* firstTransition(char*);
int analyzeInstruction(char[],char**,int);
int legalInstruction(int,int,int,int);
int getDataBinary(char[],char**,int,struct symbol*, int*);
void add100ToDataCode(struct symbol**,int);
void printSymbolTable(struct symbol*);

/* second_transition.c*/
void secondTransition(char*, struct symbol**);
int analyzeOperands(char[],struct symbol*,FILE*);
int putEntryDef(char[],struct symbol**);

/* create_obj_ext_ent.c*/
void createOb(char*);
char* binaryToEncrypted(char*);
void createEnt(char*,struct symbol*);
void createExt(char*);








