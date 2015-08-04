/*
 Author:         Joaquin Aguirre
 Class:          CS572, Fall 2013
 Assignment:     Lab #1
 Filename:       stack.c
 
 Notes:          This program simulates a stack base machine. It takes MIPS similar assembly code,
            and loads the code into memory. It will then simulate a stack machine by assesing the
            different locations in memory and places datum into the stack.
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

#define DEBUG 0

/*  DECLARE VALUES FOR OPCODE */
#define PUSH 0x0005
#define POP 0x0006
#define ADD 0x0007
#define MULT 0x0008
#define END 0xFFFF

char labels[10][4];
uint32_t label_mem[10];
uint32_t stack_pointer;
uint32_t program_counter;

/* Simulated memory layout */
struct memory_blocks{
	uint64_t user_text[ARRAY_SIZE];
	uint64_t user_data[ARRAY_SIZE];
	uint64_t stack_data[ARRAY_SIZE];
	uint64_t kernel_data[ARRAY_SIZE];
	uint64_t kernel_text[ARRAY_SIZE];
};

/*  Fuction will return a index value for the memory_block
        struct based on an address arguement.               */
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
/*  Write to a specific location in memeory, given an address.    */
void writeMemory(uint64_t* memory, uint32_t address, uint64_t data) {
	memory[addressLocation(address)] = data;
}
/*  Convert a string into an interger.          */
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
	
	while(fgets(buffer, 512, infile) != 0) {
		//Checks to see if buffer is within the .data section
		if(data == 1){
            //Copy buffer
			for(int x=0; x < 512; x++)
				temp[x] = buffer[x];
			for(int x=0; x < 512; x++)
				if(buffer[x] == ':')
					counter = x;
			temp[counter] = '\0';
			strcpy(labels[label_counter], temp);
			uint64_t label_value = str_to_integer(buffer);
			writeMemory(memory,data_pointer,label_value);
			label_mem[label_counter] = data_pointer;
			label_counter++;
			data_pointer += 64;
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
	char temp[512];
	infile = fopen(file,"r");
	int text = 0;
	int counter = 0;
	
	while(fgets(buffer, 512, infile) != 0) {
		//Get data variables 
		if(strstr(buffer,".data") != NULL) {
			text = 0;
		}
		if(text == 1){
			if(strstr(buffer, "PUSH") != NULL) {
				uint64_t temp = PUSH;
				temp = temp << 32;
				for(int x=0; x<10; x++)
					if(strstr(buffer, labels[x]) != NULL)
						temp = temp + label_mem[x];
				writeMemory(memory,program_counter,temp);
				program_counter += 64;
			}
			else if(strstr(buffer, "POP") != NULL) {
				uint64_t temp = POP;
				temp = temp << 32;
				for(int x=0; x<10; x++)
					if(strstr(buffer, labels[x]) != NULL)
						temp = temp + label_mem[x];
				writeMemory(memory,program_counter,temp);
				program_counter += 64;
			}
			else if(strstr(buffer, "MULT") != NULL) {
				uint64_t temp = MULT;
				temp = temp << 32;
				writeMemory(memory,program_counter,temp);
				program_counter += 64;
			}
			else if(strstr(buffer, "ADD") != NULL) {
				uint64_t temp = ADD;
				temp = temp << 32;
				writeMemory(memory,program_counter,temp);
				program_counter += 64;
			}
			else if(strstr(buffer,"END") != NULL) {
				uint64_t temp = END;
				temp = temp << 32;
				writeMemory(memory,program_counter,temp);
				program_counter += 64;
			}
		}
		if(strstr(buffer,".text") != NULL) {
			text = 1;
		}
	}
}

/*  Function will place data into the next location on the stack.   */
void pushStack(uint64_t* memory, uint64_t data) {
	writeMemory(memory, stack_pointer, data);
	stack_pointer -= 64;
}

/*  Fuction will retrieve data from the top of the stack and 
        place it in a location in memory (argument).                */
void popStack(uint64_t* memory, uint32_t address) {
	stack_pointer += 64;
	uint64_t temp = accessMemory(memory,stack_pointer);
	writeMemory(memory,address, temp);
}

/*  Fuction will retrieve data from the top of the stack and
        return the value. This is to use for arithmetic purposes.   */
uint64_t popStackArith(uint64_t* memory) {
	stack_pointer += 64;
	return accessMemory(memory,stack_pointer);
}

/*  The main fuction will call the functions to load the users
        assembly file into memory. Then simulate the program based
        on the instrucitons loaded into memory.
                                                                    */
int main(int argc, char* argv[]) {

	
	char *file_name = "stackCode.s";		
	struct memory_blocks* memory;
	memory = (struct memory_blocks *) malloc(sizeof(struct memory_blocks));

	if(argc != 1)
		file_name = argv[1];
	
	program_counter = START_ADDRESS;
	
	getUserData(file_name, memory->user_text);
	getUserText(file_name, memory->user_text);

	program_counter = START_ADDRESS;
	stack_pointer = STACK_DATA;
	bool user_mode = true;
	uint64_t temp1, temp2;
	uint32_t opcode;
	uint32_t address;
	
	
	while(user_mode) {
		opcode = uint32_t(accessMemory(memory->user_text,program_counter) >> 32);
		address = uint32_t(accessMemory(memory->user_text,program_counter));
		program_counter += 64;
		switch(opcode) {
		case PUSH:
			printf("PUSH: ");
			pushStack(memory->user_text, accessMemory(memory->user_text,address));
			printf("%lld\n" ,accessMemory(memory->user_text,stack_pointer+64));
			break;
		case POP:
			printf("POP:  ");
			popStack(memory->user_text, address);
			printf("%lld -> %x",accessMemory(memory->user_text,address), address);
			break;
		case ADD:
			printf("ADD:  ");
			temp1 = popStackArith(memory->user_text);
			temp2 = popStackArith(memory->user_text);
			printf("%lld + %lld = %lld\n", temp1, temp2, temp1+temp2);
			pushStack(memory->user_text,(temp1+temp2));
			break;
		case MULT:
			printf("MULT: ");
			temp1 = popStackArith(memory->user_text);
			temp2 = popStackArith(memory->user_text);
			printf("%lld * %lld = %lld\n", temp1,temp2, temp1*temp2);
			pushStack(memory->user_text,(temp1*temp2));

			break;
		case END:
			user_mode = false;
			break;
		}
	}
	printf("\n");
	return 0;
}
