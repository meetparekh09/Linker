#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int bool;

extern void openFile(char *filename);
extern void closeFile();
extern char* readToken();

extern int line_num;
extern int line_offset;

extern void pass1();
extern void pass2();

int main(int argc, char *argv[]) {
	if(argc < 2) {
		printf("Expected file name in argument\n");
		exit(0);
	}
	
	/******************************** Pass 1 ********************************/
	openFile(argv[1]);
	pass1();
	closeFile();


	/******************************** Pass 2 ********************************/
	openFile(argv[1]);
	pass2();
	closeFile();
}
