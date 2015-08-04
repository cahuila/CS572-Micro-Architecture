// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>

/*  DECLARED VARIABLES FOR SIMULATED MEMORY */
#define ARRAY_SIZE 100
#define START_ADDRESS 0x00400000
#define USER_TEXT 0x00400000
#define USER_DATA 0x01000000
#define STACK_DATA 0x7FFFFFFF
#define KERNEL_TEXT 0x80000180
#define KERNEL_DATA 0x90000000

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
#define NOP	    0x0000
#define LABEL_SIZE 20

/* Simulated memory layout */
struct memory_blocks{
	uint64_t user_text[ARRAY_SIZE];
	uint64_t user_data[ARRAY_SIZE];
	uint64_t stack_data[ARRAY_SIZE];
	uint64_t kernel_data[ARRAY_SIZE];
	uint64_t kernel_text[ARRAY_SIZE];
};



uint64_t str_to_integer(const char *string);
int readLine(FILE *f, char *buffer, size_t len);
void getUserData(char* file, uint64_t* memory);
void getUserText(char* file, uint64_t* memory);
int addressLocation(uint32_t address);
uint64_t accessMemory(uint64_t* memory, uint32_t address);
void writeMemory(uint64_t* memory, uint32_t address, uint64_t data);
