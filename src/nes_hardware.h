#ifndef _NES_HARDWARE_H__
#define _NES_HARDWARE_H__

#include"nes_utils.h"

typedef struct NES_HARDWARE_INFO
{
	p_nes_rom_info p_rom_info;
	p_nes_ppu_info p_ppu_info;
	p_nes_cpu_info p_cpu_info;
}nes_hardware_info, * p_nes_hardware_info;

#define NMI_VECTOR 0xFFFA
#define RESET_VECTOR 0xFFFC
#define IRQ_VECTOR 0xFFFE

int init_hardware(p_nes_hardware_info p_hardware_info);
int reset_hardware(p_nes_hardware_info p_hardware_info, const char* path);
void uninit_hardware(p_nes_hardware_info p_hardware_info);

int nes_mapper_000_reset(p_nes_hardware_info);

#ifdef _DEBUG

void run_cpu(p_nes_hardware_info p_hardware_info);
int render(p_nes_ppu_info info, void* data);

#endif

#endif // !_NES_HARDWARE_H__
