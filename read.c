#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *file; //file stream

int line_num; //line number of current token
int line_offset; //line offset of current token

char *line; //holds current line from file
char *token; //holds position of current token in line

extern void __parseerror(int errcode); //in errors.c

char* readToken() {
	//read a token from the file
	//returns string

	static int newline = 1; //to check whether newline is to be read from file
	static int line_len = 0; //remember current line length
	char buffer[256];

	while(1) {
		//check if newline is to be read from file
		if(newline) {
			if(fgets(buffer, 256, file)) {
				line = (char *)malloc(sizeof(char)*strlen(buffer));
				strcpy(line, buffer); //copy line from buffer
				line_len = strlen(line); 
				line_num++;
				token = strtok(line, " \t\n"); //read token

				//if token is read then set newline and offset
				if(token) {
					newline = 0;
					line_offset = (token - line) + 1;
					return token;
				}
			} else {
				//if no other line is read then set lineoffset to line length
				line_offset = line_len;
				return NULL; //no more tokens to read
			}
		} else {
			token = strtok(NULL, " \t\n");

			//if token is not null then process accordingly
			//otherwise set newline and read another line
			if(token) {
				line_offset = (token - line) + 1;
				return token;
			} else {
				newline = 1;
			}
		}
	}
}

void openFile(char *filename) {
	//Initialize file stream
	//arguments: 
	//	filename: string of filepath
	file = fopen(filename, "r");
}

void closeFile() {
	//Close currently open filestream
	fclose(file);
}

int isnumber(const char *token) {
	//Check whether the given string is a valid number
	//arguments
	//	token: string of numerics
	int len = strlen(token);

	//if empty string then return errcode 0
	if(len == 0) {
		return 0;
	} 
	//else check whether all the char are numerics
	else {
		for(int i = 0; i < len; i++) {
			if(token[i] < '0' || token[i] > '9') {
				return 0;
			}
		}
	}

	//return code 7 as max errcode value is 6
	return 7;
}

int isValidSymbol(const char *token) {
	//Check whether the given token is valid symbol
	//arguments:
	//	token: string
	int len = strlen(token);

	//if empty string then return errcode 1
	if(len == 0) {
		return 1;
	}
	//if length greater then 16 then return errcode 16
	else if(len > 16) {
		return 3;
	} 
	//otherwise check if first char is alpha and rest is alphanumeric
	else {
		if((token[0] < 'a' || token[0] > 'z') && (token[0] < 'A' || token[0] > 'Z')) {
			return 1;
		}
		for(int i = 1; i < len; i++) {
			if((token[i] < 'a' || token[i] > 'z') && (token[i] < 'A' || token[i] > 'Z') && (token[i] < '0' || token[i] > '9')) {
				return 1;
			}		
		}
	}

	//return code 7 as max errcode value is 6
	return 7;
}

int isValidAddr(const char *token) {
	//Check whether the given token is valid address
	//arguments:
	//	token: string
	int len = strlen(token);

	//check if length is 1
	if(len != 1) {
		return 2;
	}
	//check if it is valid command
	if(token[0] != 'A' && token[0] != 'I' && token[0] != 'R' && token[0] != 'E') {
		return 2;
	}

	//return code 7 as max errcode value is 6
	return 7;
}


int readNumber() {
	//Reads a number from the file
	//returns int
	char *token = readToken();

	//if token is NULL, return -1 characterizing end of file
	if(token == NULL) {
		return -1;
	}

	//check if token is a valid number and return the number else errormessage
	if(isnumber(token)) {
		return atoi(token);
	} else {
		__parseerror(0);
		exit(-1);
	}
}

char* getSymbol() {
	//read the symbol from the file
	//returns string
	char *token = readToken();

	//if token is NULL then error
	if(token == NULL) {
		__parseerror(1);
		exit(-1);
	}

	//otherwise get the code from isValidSymbol and act accordingly
	int code = isValidSymbol(token);
	if(code < 7) {
		__parseerror(code);
		exit(-1);
	} else {
		return token;
	}
}


char *getAddr() {
	//Read the address from the file
	//returns string
	char *token = readToken();

	//if token is NULL then error
	if(token == NULL) {
		__parseerror(2);
		exit(-1);
	}

	//otherwise get the code from isValidAddr and act accordingly
	int code = isValidAddr(token);
	if(code < 7) {
		__parseerror(code);
		exit(-1);
	} else {
		return token;
	}
}