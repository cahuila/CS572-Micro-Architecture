// AccumSim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*
Author:         Joaquin Aguirre
Class:          CS572, Fall 2013
Assignment:     Lab #2
Filename:       gprSim.c

Notes:          This program simulates an accumulator base machine. It takes MIPS similar assembly code,
and loads the code into memory. It will then simulate a accumulator machine by assesing the
different locations in memory and places datum into an accumualtor.
*/
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>

uint64_t str_to_integer(const char *string);
int readLine(FILE *f, char *buffer, size_t len);
void getUserData(char* file, uint64_t* memory);
void getUserText(char* file, uint64_t* memory);
int addressLocation(uint32_t address);
uint64_t accessMemory(uint64_t* memory, uint32_t address);
void writeMemory(uint64_t* memory, uint32_t address, uint64_t data);
using namespace std;

/*  DECLARED VARIABLES FOR SIMULATED MEMORY */
#define ARRAY_SIZE 100
#define START_ADDRESS 0x00400000
#define USER_TEXT 0x00400000
#define USER_DATA 0x01000000
#define STACK_DATA 0x7FFFFFFF
#define KERNEL_TEXT 0x80000180
#define KERNEL_DATA 0x90000000

/*  DECLARE VALUES FOR OPCODE */
/*  DECLARE VALUES FOR OPCODE */
#define ADD		0x0001
#define MULT	0x0002
#define LOAD	0x0003
#define STO		0x0004
#define ADDI	0x0005
#define B		0x0006
#define BEQZ	0x0007
#define BGE		0x0008
#define BNE		0x0009
#define LA		0x0010
#define LB		0x0011
#define LI		0x0012
#define SUBI	0x0013
#define SYSCALL	0x0014
#define END		0x0020
#define LABEL_SIZE 20

char labels[LABEL_SIZE][512];
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
		return 0;
	else if((address >= USER_TEXT)&&(address < USER_DATA))
		return (address - USER_TEXT)/64;
	else if((address >= USER_DATA)&&(address < 0x38000000))
		return ((address - USER_DATA)/64)+ARRAY_SIZE;
	else if(address <= STACK_DATA)
		return ((STACK_DATA - address)/64)+(ARRAY_SIZE*2);
	else
		return 0;
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
	int negative = 1;
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
			if(c == '\r') {
				buffer[x] = 0;
				return x+1;
			}
			else if (c == '\n') {
				buffer[x] = 0;
				return x+1;
			}
			else if(c == '#')
				buffer[x] = 0;
			else
				buffer[x] = c;
		}
		else
			return -1;

	}
	return 0;
}
/*  Load the user's data section into memory. */
void getUserData(char* file, uint64_t* memory) {
	FILE *infile;
	char buffer[512];
	char temp[512];
	char* temp2;
	char* data_mem;
	infile = fopen(file,"r");
	int data = 0;
	int counter = 0;
	int label_counter = 0;
	int result = 0;
	int mem_counter = 0;
	bool in_string = 0;
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

			if((temp[0] != 0) && (temp[0] != '\t')){
				if((temp2 = strstr(buffer,".asciiz")) != NULL){
					strcpy(labels[label_counter], temp);
					label_mem[label_counter] = data_pointer;
					for(int x=0; x < 512; x++) {
						if(temp2[x] == '"')
								in_string = !in_string;
						else if(in_string) {
							data_mem = (char *)(&memory[addressLocation(data_pointer)]);
							data_mem[mem_counter] = temp2[x];
							mem_counter++;
							if(temp2[x] == 0)
								break;
						}

							if(temp2[x] == 0)
								break;
					}
					data_mem[mem_counter] = 0;
					if((mem_counter % 8) == 0)
						data_pointer += (mem_counter * 8);
					else
					{
						mem_counter = (mem_counter / 8) * 64;
						mem_counter += 64;
						data_pointer += mem_counter;
					}
					mem_counter = 0;
					label_counter++;
					cout << endl;
				}
				else if((temp2 = strstr(buffer,".space")) != NULL) {
					int spaces = str_to_integer(temp2);
					data_mem = (char *)(&memory[addressLocation(data_pointer)]);
					for(int x=0; x < spaces; x++)
						data_mem[x] = 0;
					if((spaces % 8) == 0)
						data_pointer += (spaces * 8);
					else
					{
						spaces = (spaces / 8) * 64;
						spaces += 64;
						data_pointer += spaces;
					}
					mem_counter = 0;
				}
				else {
					uint64_t label_value = str_to_integer(buffer);
					writeMemory(memory,data_pointer,label_value);
					label_mem[label_counter] = data_pointer;
					label_counter++;
					data_pointer += 64;
				}
			}
		}	
		if(strstr(buffer,".data") != NULL) {
			data = 1;
		}
	}
}

uint32_t getValues(char* string) {
	int number_shifts = 27;
	int in_number = 0;
	int result = 0;
	char* temp_index;
	for(int x =0; x < 512; x++) {
		if(string[x] == '$'){
			temp_index = &string[x];
			in_number = 1;
		}
		else if(string[x] == ',') {
			if(in_number == 1){
				string[x] = 0;
				result += str_to_integer(temp_index) << number_shifts;
//				cout << str_to_integer(temp_index) << endl;
				number_shifts -= 5;
				temp_index = &string[x+1];
				in_number = 0;
			}
		}
		else if(string[x] == 0) {
			result += str_to_integer(temp_index);
//			cout << str_to_integer(temp_index) << endl;
			break;
		}
	}
	return result;
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
//		cout << buffer << endl;
		if(strstr(buffer,".data") != NULL) {
			text = 0;
		}
		if(text == 1){
			if((temp = strstr(buffer,"MULT")) != NULL) {
				temp = strstr(temp,",");
				temp1 = MULT;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++) {
					if(strstr(temp,labels[x]) != NULL){
						temp2 = label_mem[x];
						break;
					}
				}
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"LOAD")) != NULL) {
				temp = strstr(temp,",");
				temp1 = LOAD;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++) {
					if(strstr(temp,labels[x]) != NULL){
						temp2 = label_mem[x];
						break;
					}
				}
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"STO")) != NULL) {
				temp = strstr(temp,",");
				temp1 = STO;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++) {
					if(strstr(temp,labels[x]) != NULL){
						temp2 = label_mem[x];
						break;
					}
				}
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"ADDI")) != NULL) {
//				cout << buffer << endl;
				temp = strstr(temp, "$");
				temp1 = ADDI;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
//				cout << "**********" << temp1 << "**************" << endl;
			}
			else if((temp = strstr(buffer,"BEQZ")) != NULL) {
				temp = strstr(temp, "$");
				temp1 = BEQZ;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"BGE")) != NULL) {
				temp = strstr(temp, "$");
				temp1 = BGE;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"BNE")) != NULL) {
				temp = strstr(temp, "$");
				temp1 = BNE;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"LA")) != NULL) {
				temp = strstr(temp, "$");
				temp1 = LA;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"LB")) != NULL) {
				temp = strstr(temp, "$");
				temp1 = LB;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"LI")) != NULL) {
				temp = strstr(temp, "$");
				temp1 = LI;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"SUBI")) != NULL) {
				temp = strstr(temp, "$");
				temp1 = SUBI;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"SYSCALL")) != NULL) {
				temp1 = SYSCALL;
				temp1 = temp1 << 32;
				temp2 = 0;
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"END")) != NULL) {
				temp1 = END;
				temp1 = temp1 << 32;
				temp2 = 0;
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"B")) != NULL) {
				temp1 = B;
				temp1 = temp1 << 32;
				temp2 = 0;
				if(temp[2] == '-') {
					temp2 = 1;
					temp2 = temp2 << 31;
				}
				temp2 += str_to_integer(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"ADD")) != NULL) {
				temp = strstr(temp,",");
				temp1 = ADD;
				temp1 = temp1 << 32;
				for(int x=0; x<10; x++) {
					if(strstr(temp,labels[x]) != NULL){
						temp2 = label_mem[x];
						break;
					}
				}
				writeMemory(memory,program_counter, temp1 + temp2);
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

	//	char *file_name = "C:\\accumCode.s";
	char *file_name = "C:\\Users\\Jack\\Desktop\\lab2.s";
	struct memory_blocks* memory;
	memory = (struct memory_blocks *) malloc(sizeof(struct memory_blocks));

	uint32_t gpr[32];

	if(argc != 1)
		file_name = argv[1];

	program_counter = START_ADDRESS;

	getUserData(file_name, memory->user_text);
	getUserText(file_name, memory->user_text);

	int user_mode = 1;
	int instruction_counter = 0;
	int cycles = 0;
	uint32_t regDest, regSrc1, regSrc2;
	uint32_t immediate, offset, boffset, temp4;
	uint64_t temp1, temp2;
	uint64_t accum;
	uint32_t opcode;
	uint32_t address;	
	accum = 0;
	char* pointerchar;
	pointerchar = (char *)memory->user_data;
//	for(int x =0; x < 1000; x++)
//		cout << pointerchar[x] << " ";
	program_counter = START_ADDRESS;
	while(user_mode == 1) {


		address = accessMemory(memory->user_text,program_counter);
		opcode = accessMemory(memory->user_text,program_counter) >> 32;
		regDest = (address >> 27);
		regSrc1 = ((address << 5) >> 27);
		immediate =  ((address << 10) >> 10);
		offset = ((address << 5) >> 5);
		boffset = ((address << 1) >> 1);

		program_counter += 64;
		instruction_counter++;

		switch(opcode) {
		case LOAD:
			cout << "LOAD" << endl;
			break;
		case STO:
			cout << "STO" << endl;
			break;
		case SYSCALL:
			cout << "SYSCALL" << endl;
			cycles += 8;
			break;
		case ADDI:
			cout << "ADDI" << endl;
			cout << "The destination register: " << regDest << endl;
			cout << "The add register: " << regSrc1 << endl;
			cout << "The immediate: " << immediate << endl;
			gpr[regDest] = gpr[regSrc1] + immediate;
			cout << "Reg" << regDest << " = " << gpr[regDest] << endl;
			cycles += 6;
			break;
		case SUBI:
			cout << "SUBI" << endl;
			cout << "The destination register: " << regDest << endl;
			cout << "The add register: " << regSrc1 << endl;
			cout << "The immediate: " << immediate << endl;
			gpr[regDest] = gpr[regSrc1] - immediate;
			cout << "Reg" << regDest << " = " << gpr[regDest] << endl;
			cycles += 6;
			break;
		case ADD:
			cout << "ADD" << endl;
			break;
		case MULT:
			cout << "MULT" << endl;
			break;
		case LI:
			cout << "LI" << endl;
			cout << "The register used is: " << regDest << endl;
			cout << "The immediate number: " << offset << endl;
			gpr[regDest] = offset;
			cycles += 3;
			break;
		case LA:
			cout << "LA" << endl;
			cout << "The register used is: " << regDest << endl;
			cout << "The offfset used is:  " << offset << endl;
			cout << "Address: " << program_counter + offset << endl;
			gpr[regDest] = program_counter + offset;
			cycles += 5;
			break;
		case LB:
			cout << "LB" << endl;
			cout << "The destination register: " << regDest << endl;
			cout << "The address register: " << gpr[regSrc1] << endl;
			cout << "The offset: " << immediate << endl;
			pointerchar = (char *) memory->user_data;
			temp4 = (gpr[regSrc1] - USER_DATA)/8;
			pointerchar += temp4 + 1;
			cout << pointerchar << endl;
			gpr[regDest] = uint32_t(pointerchar[0]);
			cycles += 6;
			break;
		case BGE:
			cout << "BGE" << endl;
			cout << "The register one: " << regDest << endl;
			cout << "The register two: " << regSrc1 << endl;
			cout << "The offset: " << immediate << endl;
			if(gpr[regDest] == 0)
				program_counter += immediate;
			cycles += 5;
			break;
		case BNE:
			cout << "BNE" << endl;
			cout << "The register one: " << regDest << endl;
			cout << "The register two: " << regSrc1 << endl;
			cout << "The offset: " << immediate<< endl;
			if(gpr[regDest] != gpr[regSrc1])
				program_counter += immediate;
			cycles += 5;
			break;
		case BEQZ:
			cout << "BEQZ" << endl;
			cout << "The register one: " << regDest << endl;
			cout << "The register two: " << regSrc1 << endl;
			cout << "The offset: " << offset << endl;
			if(gpr[regDest] == 0)
				program_counter += offset;
			cycles += 5;
			break;
		case B:
			cout << "B" << endl;
			cout << "OLD program counter: " << program_counter << endl;
			if((address >> 31) == 1)
				program_counter = program_counter - boffset;
			else
				program_counter = program_counter + boffset;

			cout << "The offset: " << boffset << endl;
			cout << "New program counter: " << program_counter << endl;
			cycles += 4;
			break;
		case END:
			user_mode = 0;
			break;
		}
	}

	cout << "Total Instructions -> " << instruction_counter << endl;
	cout << "Total Cycles       -> " << cycles << endl;
	return 0;
}
