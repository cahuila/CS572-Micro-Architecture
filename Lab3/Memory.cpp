#include "stdafx.h"


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