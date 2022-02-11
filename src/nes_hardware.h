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

//#define VK_RIGHT(a) (a|=0x1)
//#define VK_LEFT(a) (a|=0x2)
//#define VK_DOWN(a) (a|=0x4)
//#define VK_UP(a) (a|=0x8)
//#define VK_START(a) (a|=0x10)
//#define VK_SELECT(a) (a|=0x20)
//#define VK_B(a) (a|=0x40)
//#define VK_A(a) (a|=0x80)

// reserve
#define NES_VK_RIGHT 0x80
#define NES_VK_LEFT 0x40
#define NES_VK_DOWN 0x20
#define NES_VK_UP 0x10
#define NES_VK_START 0x8
#define NES_VK_SELECT 0x4
#define NES_VK_B 0x2
#define NES_VK_A 0x1

int init_hardware(p_nes_hardware_info p_hardware_info);
int reset_hardware(p_nes_hardware_info p_hardware_info, const char* path);


void nes_input(p_nes_hardware_info p_hardware_info, ubyte joypad1, ubyte joypad2);
void nes_input_reset(p_nes_hardware_info p_hardware_info, ubyte joypad1, ubyte joypad2);


void uninit_hardware(p_nes_hardware_info p_hardware_info);

int nes_mapper_000_reset(p_nes_hardware_info);

#ifdef _DEBUG

void run_cpu(p_nes_hardware_info p_hardware_info);
int render(p_nes_ppu_info info, void* data);

#endif

#endif // !_NES_HARDWARE_H__
