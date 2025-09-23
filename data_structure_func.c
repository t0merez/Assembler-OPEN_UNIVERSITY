#include "lib.h"

struct macro* inMacroList(char *name, struct macro* head)
/*
	Returns the pointer to the macro in the macro list with the given name.
*/
{
	struct macro* curr = head;

	while(curr!=NULL)
	{
		if(strcmp(name,curr->name) == 0)
		{
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}

void addMacro(char* name, char* code, struct macro** head)
/*
	Adds a macro to the macro table.
*/
{
    	struct macro* new = (struct macro*)malloc(sizeof(struct macro));
	struct macro* curr;
    	if (new == NULL) 
	{
        	fprintf(stderr, "Memory allocation failed\n");
        	exit(1);
    	}
    
    	new->name = my_strdup(name);
    	new->code = my_strdup(code);
    	new->next = NULL;

    	if (*head == NULL) 
        	*head = new;  
	else 
	{
        	curr = *head;
        	while (curr->next != NULL) 
            		curr = curr->next;
        	
        	curr->next = new; 
    	}
}

struct symbol* getSymbol(char name[],struct symbol* head)
/*
	Returns the pointer to the symbol in the symbol table with the given name.
*/
{
	struct symbol* curr = head;

	while(curr)
	{
		if(strcmp(curr->name,name) == 0)
			return curr;
		curr = curr->next;
	}
	return NULL;
}

int inSymbolTable(char *name,struct symbol* head)
/* 
	Returns 1 if the name of the symbol is in the given symbol table and 0 if not.
*/
{
	struct symbol* curr = head;

	while(curr)
	{
		if(strcmp(curr->name,name) == 0)
			return 1;
		curr = curr->next;
	}
	return 0;
}

void addSymbol(char* name, char* feature, int val, struct symbol** head)
/*
	Adds a symbol to the given symbol table.
*/
{
    	struct symbol* new = (struct symbol*)malloc(sizeof(struct symbol));
	struct symbol* curr;
    	if (new == NULL) 
	{
        	fprintf(stderr, "Memory allocation failed\n");
        	exit(1);
    	}
    
    	new->name = my_strdup(name);
    	new->feature = my_strdup(feature);
	new->val = val;
    	new->next = NULL;

    	if (*head == NULL) 
        	*head = new;  
	else 
	{
        	curr = *head;
        	while (curr->next != NULL) 
            		curr = curr->next;
        	
        	curr->next = new; 
    	}
}
