#ifndef __NES_CPU_
#define __NES_CPU_

#include"nes_types.h"

typedef struct CPU_REGISTERS
{
	// Accumulator
	byte A;
	// Indexes.X and Y are byte-wide and used for several addressing modes. 
	byte X;
	byte Y;
	// Stack Pointer
	// the stack always uses some part of the $0100-$01FF page
	// the top is $0xFD 
	byte SP;
	// Status Register
	// P has 6 bits used by the ALU but is byte-wide. PHP, PLP, arithmetic, testing, and branch instructions can access this register.
	byte P;
	// Program Counter
	word PC;
}cpu_registers;

typedef void (*opcode_func)(struct CPU_INFO*);

typedef struct CPU_INFO
{
	cpu_registers registers;
	opcode_func opcodes[0xFF + 1];
	// 64KB
	byte* memory;
}cpu_info, * p_cpu_info;

#define RAM_START 0x0
#define ZERO_PAGE RAM_START
#define BASE_STACK 0x100
#define RAM 0x200
#define RAM_MIRRORS 0x800
#define PPU_REGISTERS 0x2000
#define PPU_REGISTERS_MIRRORS 0x2008
#define IO_REGISTERS 0x4000
#define EXPANSION_ROM 0x4020
#define SRAM 0x6000
#define PRG_ROM_L 0x8000
#define PRG_ROM_H 0xC000
// max memory address
#define CPU_RAM_MAX 0xFFFF

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
// On the NES, this flag has no effect.
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_R 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

#define NMI_VECTOR 0xFFFA
#define RESET_VECTOR 0xFFFC
#define IRQ_BRK_VECTOR 0xFFFE

int init_opcodes(p_cpu_info info);

int init_cpu();
void clean_cpu(p_cpu_info info);
//void reset_cpu();
//void cpu_step();

#endif