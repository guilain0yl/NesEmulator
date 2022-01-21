#ifndef _NES_CPU_H__
#define _NES_CPU_H__

#include"nes_utils.h"


typedef struct NES_CPU_REGISTERS
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
}nes_cpu_registers, * p_nes_cpu_registers;

typedef void (*opcode_func)(struct NES_CPU_INFO*);

typedef struct NES_CPU_INFO
{
	nes_cpu_registers registers;
	opcode_func opcodes[0xFF + 1];
	void* hardware;
}nes_cpu_info, * p_nes_cpu_info;

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
// On the NES, this flag has no effect.
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_R 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

struct FAST_TABLE
{
	ubyte value;
	ubyte flag;
};

ubyte g_flag_nz_table[256];
struct FAST_TABLE g_asl_table[256];
struct FAST_TABLE g_lsr_table[256];
struct FAST_TABLE g_rol_table[2][256];
struct FAST_TABLE g_ror_table[2][256];

void init_cpu(p_nes_cpu_info info);

#endif
