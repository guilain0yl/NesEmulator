#ifndef _NES_APU_H__
#define _NES_APU_H__

#include"nes_hardware.h"

ubyte read_apu_byte(p_nes_mem_info info, uword address);
void write_apu_byte(p_nes_mem_info info, uword address, ubyte data);

#endif