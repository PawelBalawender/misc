#include <iostream>
#include <cstring>
/**
This module fools with some low-level tasks such as
using processor's instructions for your purposes etc.

todo: implement Brainfuck interpreter on it
*/

enum instructions {
	ADD,
	PUSH,
	POP,
	EZ,
	CB,
	SWP,
	LOAD,
	PUT,
	HLT,
	MOV
};

enum registers {
	DP,
	SP,
	ACC,
	CNT,
	PC,
	STATUS
};

class CPU {
	public:
	
	enum status_bits {
		CPU_EZ   = 1,
		CPU_HALT = 2,
	};
	
	CPU(char *memory) : memory(memory) {
		accumulator = 0;
		counter = 0;
		stack_pointer = 0;
		data_pointer = 0;
		program_counter = 0;
		status = 0;
	}
	
	void run(short start_address) {
		program_counter = start_address;
		while( (status & CPU_HALT) != CPU_HALT ) {
			iterate();
		}
	}
	
	void iterate() {
		std::cerr << "GOT: " << 'a'+memory[program_counter] << "\n";
		int temp_acc;
		switch( memory[program_counter] ) {
			case LOAD:
				accumulator = memory[data_pointer];
				break;
			case ADD:
				accumulator += memory[++program_counter];
				break;
			case PUSH:
				memory[--data_pointer] = memory[++program_counter];
				break;
			case POP:
				accumulator = memory[data_pointer++];
				break;
			case EZ:
				if (accumulator == 0) {
					status |= CPU_EZ;
				} else {
					status &= ~CPU_EZ;
				}
				break;
			case CB:
				if (status & CPU_EZ == CPU_EZ) {
					// -1 only because it's incremented before execution
					program_counter = memory[++program_counter] - 1;
				}
				break;
			case PUT:
				std::cerr << "PUT: " << (char)accumulator << "\n";
				break;
			case SWP:
				temp_acc = accumulator;
				accumulator = counter;
				counter = temp_acc;
				break;
			case MOV: // 8 bit! TODO: 16 bit
				{
					char val = memory[++program_counter]; // source
					switch( memory[++program_counter] ) { // destination
						case DP:
							data_pointer = val;
					}
				}
				break;
			case HLT:
				status |= CPU_HALT;
				break;
		}
		++program_counter;
	}
		
	private:
		short accumulator;
		short counter;
		short stack_pointer;
		short data_pointer;
		short program_counter;
		short status;
		char *memory;
		
		
	
};

int main() {
	char *memory = new char[65536]; // 2<<16
	CPU cpu(memory);
	
	char prog1[] = { MOV, 6, DP, 
					 LOAD,
					 PUT,
					 HLT,
					 'x',
					 'y'
				   };
	
	memcpy(memory, prog1, sizeof(prog1));
	cpu.run(0);
	
	std::cin >> memory;
	
	delete[] memory;
	
	return 0;
};
	
