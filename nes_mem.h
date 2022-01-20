#ifndef _NES_MEM_H__
#define _NES_MEM_H__

#include"nes_utils.h"

typedef struct NES_MEM_INFO
{
	ubyte mem[0x8000];
	void* current_lower_bank_pointer;
	void* current_upper_bank_pointer;
}nes_mem_info, * p_nes_mem_info;

#define PRG_ROM_LOWER_BANK 0x8000
#define PRG_ROM_UPPER_BANK 0xC000

uword read_w(uword address, p_nes_mem_info info);

#endif // !_NES_MEM_H__
