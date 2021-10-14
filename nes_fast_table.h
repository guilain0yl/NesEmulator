#ifndef __NES_FAST_TABLE_
#define __NES_FAST_TABLE_

#include"nes_cpu.h"

struct FAST_TABLE
{
	byte value;
	byte flag;
};

byte g_flag_nz_table[256];
struct FAST_TABLE g_asl_table[256];
struct FAST_TABLE g_lsr_table[256];
struct FAST_TABLE g_rol_table[2][256];
struct FAST_TABLE g_ror_table[2][256];

#endif