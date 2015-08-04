// PipeSim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*
Author:         Joaquin Aguirre
Class:          CS572, Fall 2013
Assignment:     Lab #3
Filename:       pipeSim.c

Notes:          This program simulates a MIPS 5 stage pipeline machine. It takes MIPS similar assembly code,
and loads the code into memory. It will simulate the code going through the five stages of the MIPS  pipeline.
*/

using namespace std;

char labels[LABEL_SIZE][512];
uint32_t label_mem[10];
uint32_t program_counter;
// General purpose register
uint32_t gpr[32];
int8_t in_use[32];
int user_mode;
int instruction_count;
int cycle_count;

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
//					cout << endl;
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
				temp = strstr(temp, "$");
				temp1 = ADDI;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
			}
			else if((temp = strstr(buffer,"ADD")) != NULL) {
				temp = strstr(temp, "$");
				temp1 = ADD;
				temp1 = temp1 << 32;
				temp2 = getValues(temp);
				writeMemory(memory,program_counter, temp1 + temp2);
				program_counter += 64;
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
			else if((temp = strstr(buffer,"NOP")) != NULL) {
				temp1 = NOP;
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
		}
		if(strstr(buffer,".text") != NULL) {
			text = 1;
		}
	}
}
// Instructions for each function in the method
uint64_t ifInstruction;
uint64_t idInstruction;
uint64_t exeInstruction;
uint64_t memInstruction;
uint64_t wbInstruction;

// PC counter for each instruction
uint32_t pcID;
uint32_t pcEXE;
uint32_t pcMEM;
uint32_t pcWB;

// Buffers for regDestination register
uint32_t regDestID;
uint32_t regDestEXE;
uint32_t regDestMEM;
uint32_t regDestWB;

// Buffers for register 1
uint32_t reg1ID;
uint32_t reg1EXE;

// Buffers for register 2
uint32_t reg2ID;
uint32_t reg2EXE;

// Buffers for offset/immediate
uint32_t offID;
uint32_t offEXE;
uint32_t offMEM;
uint32_t offWB;

// ALU answer
uint32_t aluEXE;
uint32_t aluMEM;
uint32_t aluWB;

//Memory output
uint32_t memMEM;
uint32_t memWB;


// The temporary buffer for the instruction.
int programEnd;

void instructionFetch(uint64_t* memory){
		if(programEnd != 1) {
			ifInstruction = accessMemory(memory,program_counter);
			program_counter += 64;
		}
		if((ifInstruction >> 32) == END)
			programEnd = 1;

		switch((ifInstruction >> 32)){
		case B:
		case BEQZ:
		case BGE:
		case BNE:	
		case LI:
		case LA:
		case ADD:
		case ADDI:
		case SUBI:
		case NOP:
		case SYSCALL:
		case LB:
			instruction_count++;
			break;
		}

}

uint32_t register1;
uint32_t register2;
uint32_t offsetImmediate;
uint32_t terminate_program;

void instructionDecode(){
		uint32_t regDest, regSrc1;
		uint32_t immediate, offset, boffset, temp4;
		uint64_t temp1, temp2;
		uint64_t accum;
		uint32_t opcode;	
		uint32_t address;
		uint64_t instruction;
		char* pointerchar;

		instruction = idInstruction;
		address = idInstruction;
		opcode = idInstruction >> 32;
		regDest = (address >> 27);
		regSrc1 = ((address << 5) >> 27);
		immediate =  ((address << 10) >> 10);
		offset = ((address << 5) >> 5);
		boffset = address;//((address << 1) >> 1);
		int32_t reg1 = signed(reg1ID);
//		system("CLS");
//		cout << "***********************"<< endl;
//		cout << "Op: " << opcode << endl;
//		cout << "In: " << instruction << endl;
//		cout << "RD: " << regDest << endl;
//		cout << "R1: " << regSrc1 << endl;
//		cout << "Im: " << immediate << endl;
//		cout << "Of: " << offset << endl;
//		cout << "BO: " << boffset << endl;
//		cout << "***********************"<< endl << endl;

		switch(opcode){
		case B:
			if((address >> 31) == 1)
				program_counter = pcID - boffset;
			else
				program_counter = pcID + boffset;
				break;
		case LI:
		case LA:
			regDestID = regDest;
			offID = offset;
			break;
		case BEQZ:
			reg1ID = gpr[regDest];
			offID = offset;
			if(reg1ID == 0)
				program_counter = pcID + offID;
			break;
		case BGE:
			reg1ID = gpr[regDest]+1;
			reg2ID = gpr[regSrc1]+1;
			offID = immediate;
			if(reg1ID >= reg2ID)
				program_counter = pcID - offID;
			break;
		case BNE:
			reg1ID = gpr[regDest];
			reg2ID = gpr[regSrc1];
			offID = immediate;
			if(reg2ID != reg2ID)
				program_counter = pcID + offID;
			break;
		case ADD:
			reg1ID = gpr[regSrc1];
			reg2ID = gpr[immediate];
			regDestID = regDest;
			break;
		case ADDI:
		case SUBI:
		case LB:
			reg1ID = gpr[regSrc1];
			offID = immediate;
			regDestID = regDest;
			break;
		case NOP:
		case SYSCALL:
		case END:
			break;
		}
}

uint32_t exeResult;
uint32_t exeOffset;
void instructionExecute(){
		uint32_t opcode;	
		uint64_t instruction;
		
		uint32_t r1 = exeInstruction;
		uint32_t r2 = exeInstruction;
		uint32_t memTest = memInstruction;
		uint32_t wbTest = wbInstruction;

		memTest = (memTest >> 27);
		wbTest = (wbTest >> 27);

		r1 = ((r1 << 37) >> 59);
		r2 = ((r2 << 42) >> 42);

		if(r1 == regDestMEM)
			reg1EXE = aluMEM;
		else if(r1 == regDestWB)
			reg1EXE = aluWB;

		if(r2 == regDestMEM)
			reg2EXE = aluMEM;
		else if (r2 == regDestWB)
			reg2EXE = aluWB;
				
		opcode = exeInstruction >> 32;
		
		switch(opcode){
		case B:
		case BEQZ:
		case BGE:
		case BNE:	
			break;
		case LI:
			offEXE = offEXE;
			aluEXE = offEXE;
			break;
		case LA:
			offEXE = offEXE + pcEXE - 128;
			aluEXE = offEXE;
			break;
		case ADD:
			aluEXE = reg1EXE + reg2EXE;
			break;
		case ADDI:
		case LB:
			aluEXE = reg1EXE + offEXE;
			break;
		case SUBI:
			aluEXE = reg1EXE - offEXE;
			break;
		case NOP:
		case SYSCALL:
			break;
		}
}

void memoryAccess(uint64_t* memory){
		uint32_t opcode;	
		uint64_t instruction;
		
		opcode = memInstruction >> 32;

		switch(opcode){
		case B:
		case BEQZ:
		case BGE:
		case BNE:	
		case LI:
		case LA:
		case ADD:
		case ADDI:
		case SUBI:
		case NOP:
		case SYSCALL:
			break;
		case LB:
			char* pointerchar;
			pointerchar = (char *) memory;
			uint32_t temp4 = (aluMEM - USER_DATA)/8;
			pointerchar += temp4 + 1;
			cout << pointerchar << endl;
			memMEM = uint32_t(pointerchar[0]);
			break;
		}
}

void writeBack(){
		uint32_t opcode;	
		uint64_t instruction;
		opcode = wbInstruction >> 32;
		
		switch(opcode){
		case B:
		case BEQZ:
		case BGE:
		case BNE:	
		case NOP:
		case SYSCALL:
			break;
		case ADDI:
		case SUBI:
		case ADD:
			gpr[regDestWB] = aluWB;
			break;
		case LI:
		case LA:
			gpr[regDestWB] = offWB;
			break;
		case LB:
			gpr[regDestWB] = memWB;
			break;
		case END:
			user_mode = 0;
		}

}

/*  The main fuction will call the functions to load the users
assembly file into memory. Then simulate the program based
on the instrucitons loaded into memory.
*/
int main(int argc, char* argv[]) {

	//	char *file_name = "C:\\accumCode.s";
	char *file_name = "C:\\Users\\Jack\\Desktop\\lab3.s";
	struct memory_blocks* memory;
	memory = (struct memory_blocks *) malloc(sizeof(struct memory_blocks));

	if(argc != 1)
		file_name = argv[1];

	// Start program counter at Data Address
	program_counter = START_ADDRESS;
	programEnd = 0;

	// Get the User Data from the file and store into the memory
	getUserData(file_name, memory->user_text);
	
	// Get the User Text (instructions) from the file and store into the memory
	getUserText(file_name, memory->user_text);

	// Set default values for program to execute
	user_mode = 1;

	instruction_count = 0;
	cycle_count = 0;

	uint32_t regDest, regSrc1, regSrc2;
	uint32_t immediate, offset, boffset, temp4;
	uint64_t temp1, temp2;
	uint64_t accum;
	uint32_t opcode;
	uint32_t instruction;	
	char* pointerchar;
	for(int i = 0; i < 32; i++)
		in_use[i] = 0;
	int count = 0;
	accum = 0;
	pointerchar = (char *)memory->user_data;
	program_counter = START_ADDRESS;
	ifInstruction = 0;
	idInstruction = 0;
	exeInstruction = 0;
	memInstruction = 0;
	wbInstruction = 0;
	while(user_mode == 1) {

		//Copy instructions from each buffer
		wbInstruction = memInstruction;
		memInstruction = exeInstruction;
		exeInstruction = idInstruction;
		idInstruction = ifInstruction;

		//Copy pc for each instruction
		pcWB = pcMEM;
		pcMEM = pcEXE;
		pcEXE = pcID;
		pcID = program_counter;

		//Copy regDestination from each buffer
		regDestWB = regDestMEM;
		regDestMEM = regDestEXE;
		regDestEXE = regDestID;

		//Copy reg1 from each buffer
		reg1EXE = reg1ID;

		//Copy reg2 from each buffer
		reg2EXE = reg2ID;

		//Copy offset/immediate from each buffer
		offWB = offMEM;
		offMEM = offEXE;
		offEXE = offID;

		// Copy alu answer from each buffer
		aluWB = aluMEM;
		aluMEM = aluEXE;

		// Copy the memory from each buffer
		memWB = memMEM;

		instructionFetch(memory->user_text);
		instructionDecode();
		instructionExecute();
		memoryAccess(memory->user_text);
		writeBack();
		cycle_count++;

	return 0;
}
