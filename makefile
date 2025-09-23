assembler: assembler.o pre_assembler.o first_transition.o second_transition.o create_obj_ext_ent.o data_structure_func.o help_functions.o 
	gcc -g -ansi -Wall -pedantic assembler.o pre_assembler.o first_transition.o second_transition.o data_structure_func.o create_obj_ext_ent.o  help_functions.o -o assembler

assembler.o: assembler.c lib.h 
	gcc -c -g -ansi -Wall -pedantic assembler.c -o assembler.o

pre_assembler.o: pre_assembler.c lib.h 
	gcc -c -g -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o

first_transition.o: first_transition.c lib.h 
	gcc -c -g -ansi -Wall -pedantic first_transition.c -o first_transition.o

second_transition.o: second_transition.c lib.h 
	gcc -c -g -ansi -Wall -pedantic second_transition.c -o second_transition.o

create_obj_ext_ent.o: create_obj_ext_ent.c lib.h 
	gcc -c -g -ansi -Wall -pedantic create_obj_ext_ent.c -o create_obj_ext_ent.o

data_structure_func.o: data_structure_func.c lib.h 
	gcc -c -g -ansi -Wall -pedantic data_structure_func.c -o data_structure_func.o

help_functions.o: help_functions.c lib.h 
	gcc -c -g -ansi -Wall -pedantic help_functions.c -o help_functions.o








