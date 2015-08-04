/*
 Author:         Joaquin Aguirre
 Class:          CS572, Fall 2013
 Assignment:     Lab #1
 Filename:       accumulator.c
 
 Notes:          This program simulates an accumulator base machine. It takes MIPS similar assembly code,
 and loads the code into memory. It will then simulate a accumulator machine by assesing the
 different locations in memory and places datum into an accumualtor.
 */
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>


using namespace std;

/*  DECLARED VARIABLES FOR SIMULATED MEMORY */
#define ARRAY_SIZE 100
#define START_ADDRESS 0x00400000
#define USER_TEXT 0x00400000
#define USER_DATA 0x10000000
#define STACK_DATA 0x7FFFFFFF
#define KERNEL_TEXT 0x80000180
#define KERNEL_DATA 0x90000000

/*  DECLARE VALUES FOR OPCODE */
#define ADD 0x0007
#define MULT 0x0008
#define END 0x0020
#define LOAD 0x0009
#define STO 0x0010

char labels[10][4];
uint32_t label_mem[10];
uint32_t program_counter;

/* Simulated memory layout */
struct memory_blocks{
	uint64_t user_text[ARRAY_SIZE];
	uint64_t user_data[ARRAY_SIZE];
	uint64_t stack_data[ARRAY_SIZE];
	uint64_t kernel_data[ARRAY_SIZE];
	uint64_t kernel_text[ARRAY_SIZE];
};

/*  Function will return a index value for the memory_block
 struct based on an address argument.               */
int addressLocation(uint32_t address) {
	if(address < USER_TEXT)
		return -1;
	else if((address >= USER_TEXT)&&(address < USER_DATA))
		return (address - USER_TEXT)/64;
	else if((address >= USER_DATA)&&(address < 0x38000000))
		return ((address - USER_DATA)/64)+100;
	else if(address <= STACK_DATA)
		return ((STACK_DATA - address)/64)+200;
	else
		return -1;
}
/*  Return value in memory given an address.                */
uint64_t accessMemory(uint64_t* memory, uint32_t address) {
	return memory[addressLocation(address)];
}

/*  Write to a specific location in memory, given an address.    */
void writeMemory(uint64_t* memory, uint32_t address, uint64_t data) {
	memory[addressLocation(address)] = data;
}

/*  Convert a string into an integer.          */
uint64_t str_to_integer(const char *string) {
	const char *integers = "0123456789";
	int result = 0;
	
	for(; *string; ++string) {
		int int_value;
		
		for(int_value = 0; integers[int_value]; ++int_value) {
			if(integers[int_value] == *string)
			break;
		}
		
		if(integers[int_value])
			result = 10 * result + int_value;		
	}
	
	return result;
}

int readLine(FILE *f, char *buffer, size_t len) {
	char c;
	int x;
	memset(buffer,0,len);
	
	for(x=0; x <len; x++) {
		int c = fgetc(f);
		if(!feof(f)) {
			if(c == 'r')
				buffer[x] = 0;
			else if (c == '\n') {
				buffer[x] = 0;
				return x+1;
			}
			else
				buffer[x] = c;
		}
		else
			return -1;
	
	}
	return -1;
}
/*  Load the user's data section into memory. */
void getUserData(char* file, uint64_t* memory) {
	FILE *infile;
	char buffer[512];
	char temp[512];
	infile = fopen(file,"r");
	int data = 0;
	int counter = 0;
	int label_counter = 0;
	uint32_t data_pointer = USER_DATA;
	
	while(readLine(infile, buffer, 512) != -1) {
		if(data == 1){	
			for(int x=0; x < 512; x++)
				temp[x] = buffer[x];
			for(int x=0; x < 512; x++)
				if(buffer[x] == ':')
					counter = x;
			temp[counter] = 0;
			for(int x=counter; x<512; x++)
				temp[x] = 0;
				
			if(temp[0] != 0) {
				
				strcpy(labels[label_counter], temp);
			
				uint64_t label_value = str_to_integer(buffer);
				writeMemory(memory,data_pointer,label_value);
				label_mem[label_counter] = data_pointer;
				label_counter++;
				data_pointer += 64;
			}
		}	
		if(strstr(buffer,".data") != NULL) {
			data = 1;
		}
	}
}
/*  Load the user's text section into memory.   */
void getUserText(char* file, uint64_t* memory) {
	FILE *infile;
	char buffer[512];
	char* temp;
	infile = fopen(file,"r");
	int text = 0;
	int counter = 0;
	uint64_t temp1, temp2;
	
	while(readLine(infile, buffer, 20) != -1) {

		if(strstr(buffer,".data") != NULL) {
			text = 0;
		}
		if(text == 1){
			if((temp = strstr(buffer, "MULT")) != NULL) {
				temp = strstr(temp,",");
				temp1 = MULT;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++) {
					if(strstr(temp,labels[x]) != NULL){
						temp2 = label_mem[x];
						break;
					}
				}
				writeMemory(memory,program_counter,temp1+temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer, "ADD")) != NULL) {
				temp = strstr(temp,",");
				temp1 = ADD;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++) {
					if(strstr(temp,labels[x]) != NULL){
						temp2 = label_mem[x];
						break;
					}
				}
				writeMemory(memory,program_counter,temp1+temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer, "LOAD")) != NULL) {
				temp = strstr(temp,",");
				temp1 = LOAD;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++) {
					if(strstr(temp,labels[x]) != NULL){
						temp2 = label_mem[x];
						break;
					}
				}
				writeMemory(memory,program_counter,temp1+temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer, "STO")) != NULL) {
				temp = strstr(temp,",");
				temp1 = STO;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++) {
					if(strstr(temp,labels[x]) != NULL){
						temp2 = label_mem[x];
						break;
					}
				}
				writeMemory(memory,program_counter,temp1+temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"END")) != NULL) {
				temp1 = END;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++)
					if(strstr(buffer, labels[x]) != NULL)
						temp2 = label_mem[x];
				writeMemory(memory,program_counter,temp1+temp2);
				program_counter += 64;
			}
		}
		if(strstr(buffer,".text") != NULL) {
			text = 1;
		}
	}
}
/*  The main fuction will call the functions to load the users
 assembly file into memory. Then simulate the program based
 on the instrucitons loaded into memory.
                                                            */
int main(int argc, char* argv[]) {

	
	char *file_name = "accumCode.s";
	struct memory_blocks* memory;
	memory = (struct memory_blocks *) malloc(sizeof(struct memory_blocks));

	if(argc != 1)
		file_name = argv[1];
	
	program_counter = START_ADDRESS;
	
	getUserData(file_name, memory->user_text);
	getUserText(file_name, memory->user_text);
	
	int user_mode = 1;
	uint64_t temp1, temp2;
	uint64_t accum;
	uint32_t opcode;
	uint32_t address;	

	
	program_counter = START_ADDRESS;
	while(user_mode == 1) {
		
		address = accessMemory(memory->user_text,program_counter);
		opcode = accessMemory(memory->user_text,program_counter) >> 32;

		program_counter += 64;
		
		switch(opcode) {
		case LOAD:
			printf("LOAD -> ");
			accum = accessMemory(memory->user_text,address);
			cout << accum << endl;
			break;
		case STO:
			printf("STO     ");
			writeMemory(memory->user_text,address,accum);
			cout << accum <<" -> " << address << endl;
			break;
		case ADD:
			printf("ADD:    ");
			accum += accessMemory(memory->user_text,address);
			cout <<accum << endl;
			break;
		case MULT:
			printf("MULT:   ");
			accum = accessMemory(memory->user_text,address) * accum;
			cout << accum << endl;
			break;
		case END:
			user_mode = 0;
			break;
		}
	}
	printf("\n");
	return 0;
}
