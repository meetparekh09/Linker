#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int readNumber(); //in read.c
extern char* getSymbol(); //in read.c
extern char* getAddr(); //in read.c

extern void __parseerror(int errcode); //in errors.c

int base_addr; //holds base_addr of current module
int mod_num; //holds current module number

char **syms; //symbols
int *sym_val; //their corresponding value
int *sym_mod_no; //their module number
int num_sym; //total number of symbols till now
int *sym_used; //whether this symbol is used in file or not
char **sym_err; //Error message related to symbols
int num_defs_used; //among all the definitions how many are valid and used in sym table


int sym_exists(char *sym) {
	//check whether the sym already exists
	//arguments:
	//	sym: string
	//returns sym entry from sym table if exists otherwise returns -1
	for(int i = 0; i < num_sym; i++) {
		if(strcmp(syms[i], sym) == 0) {
			return i;
		}
	}
	return -1;
}


void set_sym(char *sym, int val) {
	//take the symbol and update the symbol table
	//arguments:
	//	sym: string, symbol name
	//	val: int, symbol val

	//check whether sym exists and update the symbol table or set the sym_err entry
	int sym_no = sym_exists(sym);
	if(sym_no == -1) {
		syms[num_sym] = (char *)malloc(strlen(sym)*sizeof(char));
		strcpy(syms[num_sym], sym);
		sym_val[num_sym] = val;
		sym_mod_no[num_sym] = mod_num;
		sym_used[num_sym] = 0;
		sym_err[num_sym] = malloc(100*sizeof(char));
		strcpy(sym_err[num_sym], "");
		num_sym++;
		syms = (char **)realloc(syms, (num_sym+1)*sizeof(char *));
		sym_val = (int *)realloc(sym_val, (num_sym+1)*sizeof(int));
		sym_mod_no = (int *)realloc(sym_mod_no, (num_sym+1)*sizeof(int));
		sym_used = (int *)realloc(sym_used, (num_sym+1)*sizeof(int));
		sym_err = (char **)realloc(sym_err, (num_sym+1)*sizeof(char *));
		num_defs_used++;
	} else {
		strcpy(sym_err[sym_no], " Error: This variable is multiple times defined; first value used");
	}
}

void pass1() {
	//first pass of linker

	int opcode = 0;
	int operand = 0;
	int num_defs;
	int val;
	int num_use;
	int num_instr;
	int tot_count = 0;
	num_defs_used = 0;
	char *sym;

	base_addr = 0;
	mod_num = 0;

	//Initialize symbol table
	num_sym = 0;
	syms = (char **)malloc((num_sym+1)*sizeof(char *));
	sym_val = (int *)malloc((num_sym+1)*sizeof(int));
	sym_mod_no = (int *)malloc((num_sym+1)*sizeof(int));
	sym_used = (int *)malloc((num_sym+1)*sizeof(int));
	sym_err = (char **)malloc((num_sym+1)*sizeof(char *));

	num_defs = readNumber();
	while(num_defs != -1) {
		num_defs_used = 0;
		mod_num += 1;
		if(num_defs > 16) {
			__parseerror(4);
			exit(-1);
		}
		for(int i = 0; i < num_defs; i++) {
			sym = getSymbol();
			val = readNumber();
			if(val == -1) {
				__parseerror(0);
				exit(-1);
			}
			set_sym(sym, val);
		}

		num_use = readNumber();
		if(num_use > 16) {
			__parseerror(5);
			exit(-1);
		}
		if(num_use == -1) {
			__parseerror(0);
			exit(-1);
		}		
		for(int i = 0; i < num_use; i++) {
			sym = getSymbol();
		}

		num_instr = readNumber();
		if(num_instr == -1) {
			__parseerror(0);
			exit(-1);
		}
		tot_count += num_instr;
		if(tot_count > 512) {
			__parseerror(6);
			exit(-1);
		}
		for(int i = 0; i < num_instr; i++) {
			sym = getAddr();
			val = readNumber();
			if(val == -1) {
				__parseerror(0);
				exit(-1);
			}
			opcode = val / 1000;
			operand = val % 1000;
		}


		for(int i = num_sym - 1; i >= num_sym - num_defs_used; i--) {
			if(sym_val[i] >= num_instr) {
				printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", mod_num, syms[i], sym_val[i], num_instr-1);
				sym_val[i] = base_addr;
			} else {
				sym_val[i] += base_addr;
			}
		}

		base_addr += num_instr;
		num_defs = readNumber();
	}

	printf("Symbol Table\n");
	for(int i = 0; i < num_sym; i++) {
		printf("%s=%d%s\n", syms[i], sym_val[i], sym_err[i]);
	}
	printf("\n");
	
} 



void pass2() {
	//second pass of linker
	int opcode = 0;
	int operand = 0;
	int num_defs;
	int val;
	int num_instr;
	int mem_count = 0;
	int flag = 0;
	char *sym;

	base_addr = 0;
	mod_num = 0;

	//Use table
	int num_use;
	char **use_syms;
	int *use_val;
	int *use_sym_used;

	//Warnings to be notified in the end
	char **warnings;
	int num_warning = 0;
	warnings = (char**)malloc((num_warning+1)*sizeof(char*));


	printf("Memory Map\n");
	num_defs = readNumber();
	while(num_defs != -1) {
		mod_num += 1;
		for(int i = 0; i < num_defs; i++) {
			sym = getSymbol();
			val = readNumber();
		}

		num_use = readNumber();
		use_syms = (char **) malloc((num_use)*sizeof(char*));
		use_val = (int *) malloc((num_use)*sizeof(int));
		use_sym_used = (int *) malloc((num_use)*sizeof(int));
		for(int i = 0; i < num_use; i++) {
			sym = getSymbol();
			use_syms[i] = (char*)malloc(strlen(sym)*sizeof(char));
			strcpy(use_syms[i], sym);

			int code = sym_exists(sym);
			if(code == -1) {
				use_val[i] = -1;
			} else {
				use_val[i] = sym_val[code];
				sym_used[code] += 1;
			}	
			use_sym_used[i] = 0;	
		}

		num_instr = readNumber();
		for(int i = 0; i < num_instr; i++) {
			sym = getAddr();
			val = readNumber();
			opcode = val / 1000;
			operand = val % 1000;
			if(opcode > 9) {
				flag = 1;
				opcode = 9;
				operand = 999;
			}
			if(strcmp(sym, "I") == 0) {
				printf("%03d: %d%03d", mem_count, opcode, operand);
				if(flag) {
					printf(" Error: Illegal immediate value; treated as 9999");
					flag = 0;
				}
			} else if(strcmp(sym, "A") == 0) {
				if(operand >= 512 && flag == 0) {
					printf("%03d: %d%03d", mem_count, opcode, 0);
					printf(" Error: Absolute address exceeds machine size; zero used");
				} else {
					printf("%03d: %d%03d", mem_count, opcode, operand);
				}
			} else if(strcmp(sym, "R") == 0) {
				if(operand >= num_instr && flag == 0) {
					printf("%03d: %d%03d", mem_count, opcode, base_addr);	
					printf(" Error: Relative address exceeds module size; zero used");
				} else {
					printf("%03d: %d%03d", mem_count, opcode, operand + base_addr);
				}
			} else {
				use_sym_used[operand] += 1;
				if(operand >= num_use) {
					printf("%03d: %d%03d", mem_count, opcode, operand);
					printf(" Error: External address exceeds length of uselist; treated as immediate");
				}
				else if(use_val[operand] == -1) {
					printf("%03d: %d%03d", mem_count, opcode, 0);
					printf(" Error: %s is not defined; zero used", use_syms[operand]);
				} else {
					printf("%03d: %d%03d", mem_count, opcode, use_val[operand]);
				}
			}

			if(flag) {
				printf(" Error: Illegal opcode; treated as 9999\n");
			} else {
				printf("\n");
			}
			mem_count++;
		}

		for(int i = 0; i < num_use; i++) {
			if(use_sym_used[i] == 0) {
				warnings[num_warning] = (char*)malloc(100*sizeof(char));
				sprintf(warnings[num_warning], "Warning: Module %d: %s appeared in the uselist but was not actually used", mod_num, use_syms[i]);
				num_warning++;
				warnings = (char**)realloc(warnings, (num_warning+1)*sizeof(char*));
			}
		}
		base_addr += num_instr;

		num_defs = readNumber();
	}

	printf("\n");
	for(int i = 0; i < num_sym; i++) {
	 	if(sym_used[i] == 0) {
	 		printf("Warning: Module %d: %s was defined but never used\n", sym_mod_no[i], syms[i]);
	 	}
	}

	for(int i = 0; i < num_warning; i++) {
		printf("%s\n", warnings[i]);
	}
	
} 