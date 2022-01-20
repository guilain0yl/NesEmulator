#ifndef _NES_CPU_H__
#define _NES_CPU_H__

#include"nes_utils.h"


typedef struct NES_CPU_INFO
{
	// The program counter is a 16-bit register which holds the address of the next instruction to be 
	// executed.As instructions are executed, the value of the program counter is updated, usually
	//	moving on to the next instruction in the sequence.The value can be affected by branchand
	//	jump instructions, procedure callsand interrupts.
	uword PC;
	// The stack is located at memory locations $0100-$01FF. The stack pointer is an 8-bit register 
	// which serves as an offset from $0100.The stack works top - down, so when a byte is pushed
	//	on to the stack, the stack pointer is decrementedand when a byte is pulled from the stack,
	//	the stack pointer is incremented.There is no detection of stack overflowand the stack
	//	pointer will just wrap around from $00 to $FF.
	ubyte SP;
	// The accumulator is an 8-bit register which stores the results of arithmetic and logic 
	// operations.The accumulator can also be set to a value retrieved from memory.
	ubyte A;
	// The X register is an 8-bit register typically used as a counter or an offset for certain 
	// addressing modes.The X register can be set to a value retrieved from memoryand can be
	//	used to get or set the value of the stack pointer.
	ubyte X;
	// The X register is an 8-bit register typically used as a counter or an offset for certain 
	// addressing modes.The X register can be set to a value retrieved from memoryand can be
	//	used to get or set the value of the stack pointer.
	ubyte Y;
	ubyte P;
}nes_cpu_info, * p_nes_cpu_info;

#endif
