#ifndef _NES_CPU_H__
#define _NES_CPU_H__

#include"nes_hardware.h"

struct NES_CPU_REGISTERS
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
};

typedef void (*opcode_func)(struct NES_CPU_INFO*);

struct NES_CPU_INFO
{
	struct NES_CPU_REGISTERS registers;
	opcode_func opcodes[0xFF + 1];
	p_nes_mem_info memory;
	long long clock;

	p_nes_hardware_info hardware;
};

#ifdef _DEBUG
static char opcodes_name[0xFF + 1][16] = { {'B','R','K'},
{'O','R','A','_','I','Z','X'},
{'S','L','O','_','I','Z','X'},
{' '},
{'N','O','P','_','Z','P'},
{'O','R','A','_','Z','P'},
{'A','S','L','_','Z','P'},
{'S','L','O','_','Z','P'},
{'P','H','P'},
{'O','R','A','_','I','M','M'},
{'A','S','L'},
{'A','N','C','_','I','M','M'},
{'N','O','P','_','A','B','S'},
{'O','R','A','_','A','B','S'},
{'A','S','L','_','A','B','S'},
{'S','L','O','_','A','B','S'},
{'B','P','L','_','R','E','L'},
{'O','R','A','_','I','Z','Y'},
{'S','L','O','_','I','Z','Y'},
{' '},
{'N','O','P','_','Z','P','X'},
{'O','R','A','_','Z','P','X'},
{'A','S','L','_','Z','P','X'},
{'S','L','O','_','Z','P','X'},
{'C','L','C'},
{'O','R','A','_','A','B','Y'},
{'N','O','P'},
{'S','L','O','_','A','B','Y'},
{'N','O','P','_','A','B','X'},
{'O','R','A','_','A','B','X'},
{'A','S','L','_','A','B','X'},
{'S','L','O','_','A','B','X'},
{'J','S','R','_','A','B','S'},
{'A','N','D','_','I','Z','X'},
{'R','L','A','_','I','Z','X'},
{' '},
{'B','I','T','_','Z','P'},
{'A','N','D','_','Z','P'},
{'R','O','L','_','Z','P'},
{'R','L','A','_','Z','P'},
{'P','L','P'},
{'A','N','D','_','I','M','M'},
{'R','O','L'},
{'A','N','C','_','I','M','M'},
{'B','I','T','_','A','B','S'},
{'A','N','D','_','A','B','S'},
{'R','O','L','_','A','B','S'},
{'R','L','A','_','A','B','S'},
{'B','M','I','_','R','E','L'},
{'A','N','D','_','I','Z','Y'},
{'R','L','A','_','I','Z','Y'},
{' '},
{'N','O','P','_','Z','P','X'},
{'A','N','D','_','Z','P','X'},
{'R','O','L','_','Z','P','X'},
{'R','L','A','_','Z','P','X'},
{'S','E','C'},
{'A','N','D','_','A','B','Y'},
{'N','O','P'},
{'R','L','A','_','A','B','Y'},
{'N','O','P','_','A','B','X'},
{'A','N','D','_','A','B','X'},
{'R','O','L','_','A','B','X'},
{'R','L','A','_','A','B','X'},
{'R','T','I'},
{'E','O','R','_','I','Z','X'},
{' '},
{'S','R','E','_','I','Z','X'},
{'N','O','P','_','Z','P'},
{'E','O','R','_','Z','P'},
{'L','S','R','_','Z','P'},
{'S','R','E','_','Z','P'},
{'P','H','A'},
{'E','O','R','_','I','M','M'},
{'L','S','R'},
{'A','L','R','_','I','M','M'},
{'J','M','P','_','A','B','S'},
{'E','O','R','_','A','B','S'},
{'L','S','R','_','A','B','S'},
{'S','R','E','_','A','B','S'},
{'B','V','C','_','R','E','L'},
{'E','O','R','_','I','Z','Y'},
{'S','R','E','_','I','Z','Y'},
{' '},
{'N','O','P','_','Z','P','X'},
{'E','O','R','_','Z','P','X'},
{'L','S','R','_','Z','P','X'},
{'S','R','E','_','Z','P','X'},
{'C','L','I'},
{'E','O','R','_','A','B','Y'},
{'N','O','P'},
{'S','R','E','_','A','B','Y'},
{'N','O','P','_','A','B','X'},
{'E','O','R','_','A','B','X'},
{'L','S','R','_','A','B','X'},
{'S','R','E','_','A','B','X'},
{'R','T','S'},
{'A','D','C','_','I','Z','X'},
{'R','R','A','_','I','Z','X'},
{' '},
{'N','O','P','_','Z','P'},
{'A','D','C','_','Z','P'},
{'R','O','R','_','Z','P'},
{'R','R','A','_','Z','P'},
{'P','L','A'},
{'A','D','C','_','I','M','M'},
{'R','O','R'},
{'A','R','R','_','I','M','M'},
{'J','M','P','_','I','N','D'},
{'A','D','C','_','A','B','S'},
{'R','O','R','_','A','B','S'},
{'R','R','A','_','A','B','S'},
{'B','V','S','_','R','E','L'},
{'A','D','C','_','I','Z','Y'},
{'R','R','A','_','I','Z','Y'},
{' '},
{'N','O','P','_','Z','P','X'},
{'A','D','C','_','Z','P','X'},
{'R','O','R','_','Z','P','X'},
{'R','R','A','_','Z','P','X'},
{'S','E','I'},
{'A','D','C','_','A','B','Y'},
{'N','O','P'},
{'R','R','A','_','A','B','Y'},
{'N','O','P','_','A','B','X'},
{'A','D','C','_','A','B','X'},
{'R','O','R','_','A','B','X'},
{'R','R','A','_','A','B','X'},
{'N','O','P','_','I','M','M'},
{'S','T','A','_','I','Z','X'},
{'N','O','P','_','I','M','M'},
{'S','A','X','_','I','Z','X'},
{'S','T','Y','_','Z','P'},
{'S','T','A','_','Z','P'},
{'S','T','X','_','Z','P'},
{'S','A','X','_','Z','P'},
{'D','E','Y'},
{'N','O','P','_','I','M','M'},
{'T','A','X'},
{'X','A','A','_','I','M','M'},
{'S','T','Y','_','A','B','S'},
{'S','T','A','_','A','B','S'},
{'S','T','X','_','A','B','S'},
{'S','A','X','_','A','B','S'},
{'B','C','C','_','R','E','L'},
{'S','T','A','_','I','Z','Y'},
{'A','H','X','_','I','Z','Y'},
{' '},
{'S','T','Y','_','Z','P','X'},
{'S','T','A','_','Z','P','X'},
{'S','T','X','_','Z','P','Y'},
{'S','A','X','_','Z','P','Y'},
{'T','Y','A'},
{'S','T','A','_','A','B','Y'},
{'T','X','S'},
{'T','A','S','_','A','B','Y'},
{'S','H','Y','_','A','B','X'},
{'S','T','A','_','A','B','X'},
{'S','H','X','_','A','B','Y'},
{'A','H','X','_','A','B','Y'},
{'L','D','Y','_','I','M','M'},
{'L','D','A','_','I','Z','X'},
{'L','D','X','_','I','M','M'},
{'L','A','X','_','I','Z','X'},
{'L','D','Y','_','Z','P'},
{'L','D','A','_','Z','P'},
{'L','D','X','_','Z','P'},
{'L','A','X','_','Z','P'},
{'T','A','Y'},
{'L','D','A','_','I','M','M'},
{'T','A','X'},
{'L','A','X','_','I','M','M'},
{'L','D','Y','_','A','B','S'},
{'L','D','A','_','A','B','S'},
{'L','D','X','_','A','B','S'},
{'L','A','X','_','A','B','S'},
{'B','C','S','_','R','E','L'},
{'L','D','A','_','I','Z','Y'},
{'L','A','X','_','I','Z','Y'},
{' '},
{'L','D','Y','_','Z','P','X'},
{'L','D','A','_','Z','P','X'},
{'L','D','X','_','Z','P','Y'},
{'L','A','X','_','Z','P','Y'},
{'C','L','V'},
{'L','D','A','_','A','B','Y'},
{'T','S','X'},
{'L','A','S','_','A','B','Y'},
{'L','D','Y','_','A','B','X'},
{'L','D','A','_','A','B','X'},
{'L','D','X','_','A','B','Y'},
{'L','A','X','_','A','B','Y'},
{'C','P','Y','_','I','M','M'},
{'C','M','P','_','I','Z','X'},
{'N','O','P','_','I','M','M'},
{'D','C','P','_','I','Z','X'},
{'C','P','Y','_','Z','P'},
{'C','M','P','_','Z','P'},
{'D','E','C','_','Z','P'},
{'D','C','P','_','Z','P'},
{'I','N','Y'},
{'C','M','P','_','I','M','M'},
{'D','E','X'},
{'A','X','S','_','I','M','M'},
{'C','P','Y','_','A','B','S'},
{'C','M','P','_','A','B','S'},
{'D','E','C','_','A','B','S'},
{'D','C','P','_','A','B','S'},
{'B','N','E','_','R','E','L'},
{'C','M','P','_','I','Z','Y'},
{'D','C','P','_','I','Z','Y'},
{' '},
{'N','O','P','_','Z','P','X'},
{'C','M','P','_','Z','P','X'},
{'D','E','C','_','Z','P','X'},
{'D','C','P','_','Z','P','X'},
{'C','L','D'},
{'C','M','P','_','A','B','Y'},
{'N','O','P'},
{'D','C','P','_','A','B','Y'},
{'N','O','P','_','A','B','X'},
{'C','M','P','_','A','B','X'},
{'D','E','C','_','A','B','X'},
{'D','C','P','_','A','B','X'},
{'C','P','X','_','I','M','M'},
{'S','B','C','_','I','Z','X'},
{'N','O','P','_','I','M','M'},
{'I','S','C','_','I','Z','X'},
{'C','P','X','_','Z','P'},
{'S','B','C','_','Z','P'},
{'I','N','C','_','Z','P'},
{'I','S','C','_','Z','P'},
{'I','N','X'},
{'S','B','C','_','I','M','M'},
{'N','O','P'},
{'S','B','C','_','I','M','M'},
{'C','P','X','_','A','B','S'},
{'S','B','C','_','A','B','S'},
{'I','N','C','_','A','B','S'},
{'I','S','C','_','A','B','S'},
{'B','E','Q','_','R','E','L'},
{'S','B','C','_','I','Z','Y'},
{'I','S','C','_','I','Z','Y'},
{' '},
{'N','O','P','_','Z','P','X'},
{'S','B','C','_','Z','P','X'},
{'I','N','C','_','Z','P','X'},
{'I','S','C','_','Z','P','X'},
{'S','E','D'},
{'S','B','C','_','A','B','Y'},
{'N','O','P'},
{'I','S','C','_','A','B','Y'},
{'N','O','P','_','A','B','X'},
{'S','B','C','_','A','B','X'},
{'I','N','C','_','A','B','X'},
{'I','S','C','_','A','B','X'},
};
#endif

struct FAST_TABLE
{
	ubyte value;
	ubyte flag;
};
static ubyte g_flag_nz_table[256];
static struct FAST_TABLE g_asl_table[256];
static struct FAST_TABLE g_lsr_table[256];
static struct FAST_TABLE g_rol_table[2][256];
static struct FAST_TABLE g_ror_table[2][256];

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
// On the NES, this flag has no effect.
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_R 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

int init_cpu(p_nes_cpu_info info);
void cpu_run(p_nes_cpu_info info);
int reset_cpu(p_nes_cpu_info info);
void uninit_cpu(p_nes_cpu_info info);

#endif
