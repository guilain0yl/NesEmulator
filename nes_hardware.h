#ifndef _NES_HARDWARE_H__
#define _NES_HARDWARE_H__

#include"nes_rom.h"
#include"nes_mem.h"
#include"nes_cpu.h"

typedef struct NES_HARDWARE_INFO
{
	p_nes_rom_info rom_info;
	p_nes_mem_info cpu_info;
	p_nes_mem_info mem_info;
}nes_hardware_info, * p_nes_hardware_info;

#define NMI_VECTOR 0xFFFA
#define RESET_VECTOR 0xFFFC
#define IRQ_VECTOR 0xFFFE

#endif // !_NES_HARDWARE_H__
