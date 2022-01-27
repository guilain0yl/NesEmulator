#ifndef _NES_UTILS_H__
#define _NES_UTILS_H__

typedef char sbyte;
typedef unsigned char ubyte;
typedef short sword;
typedef unsigned short uword;

typedef struct NES_ROM_INFO nes_rom_info, * p_nes_rom_info;
typedef struct NES_MEM_INFO nes_mem_info, * p_nes_mem_info;
typedef struct NES_CPU_INFO nes_cpu_info, * p_nes_cpu_info;
typedef struct NES_PPU_INFO nes_ppu_info, * p_nes_ppu_info;

#define __KB__ 1024
#define _4_KB_ (4*__KB__)
#define _8_KB_ (8*__KB__)
#define _16_KB_ (16*__KB__)

#define NES_SUCCESS 0

#define NES_NULL_POINT_ERROR -1
#define NES_ROM_NOT_FOUND_ERROR -2
#define NES_MEMORRY_ERROR -3
#define NES_ROM_READ_ERROR -4
#define NES_ROM_FORMAT_UNSUPPORT_ERROR -5
#define NES_ROM_VERSION_UNSUPPORT_ERROR -6

#define NES_MAPPER_PRG_COUNT_ERROR -7

#define NES_INIT_HARDWARE_ERROR -8

#define NES_MAPPER_NOT_FOUND_ERROR -9

#pragma warning(disable:4996)

#endif