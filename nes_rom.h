#ifndef _NES_ROM_H__
#define _NES_ROM_H__

#include"nes_utils.h"

// https://wiki.nesdev.org/w/index.php?title=INES
typedef struct ROM_HEADER
{
	// 0x4E 0x45 0x53 0x1A
	ubyte constant[4];
	// Size of PRG ROM in 16 KB units
	ubyte prg_size;
	// Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
	ubyte chr_size;
	// Mapper, mirroring, battery, trainer
	ubyte flag_6;
	// Mapper, VS/Playchoice, NES 2.0
	ubyte flag_7;
	// PRG-RAM size (rarely used extension)
	ubyte flag_8;
	// TV system (rarely used extension)
	ubyte flag_9;
	// TV system, PRG-RAM presence (unofficial, rarely used extension)
	ubyte flag_10;
	// Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
	ubyte flag_11, flag_12, flag_13, flag_14, flag_15;

}rom_header, * p_rom_header;

typedef struct NES_ROM_INFO
{
	p_rom_header rom_header;
	void* prg_data;
	void* chr_data;
	void* trainer_data;
	ubyte mapper_number;
}nes_rom_info, * p_nes_rom_info;

int load_rom(const char* path, p_nes_rom_info info);
int unload_rom(p_nes_rom_info info);

//https://wiki.nesdev.org/w/index.php?title=INES

// 0: horizontal (vertical arrangement) (CIRAM A10 = PPU A11)
// 1: vertical (horizontal arrangement) (CIRAM A10 = PPU A10)
#define MIRROR_MODE_VERTICAL(flag6) (flag6&0x1)
#define MIRROR_MODE_HORIZONTAL(flag6) (!MIRROR_MODE_VERTICAL(flag6))
// 1: Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory
// UNROM 512 and GTROM use flash memory to store their game state by rewriting the PRG-ROM area.
#define BATTERY_BACKED(flag6) (flag6&0x2)
// 1: 512-byte trainer at $7000-$71FF (stored before PRG data)
#define TRAINER(flag6) (flag6&0x4)
// 1: Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM
#define FOUR_SCREEN_VRAM(flag6) (flag6&0x8)

// VS Unisystem
#define VS_UNISYSTEM(flag7) (flag7&0x1)
// PlayChoice-10 (8KB of Hint Screen data stored after CHR data)
#define PLAY_CHOICE(flag7) (flag7&0x2)
// If equal to 2, flags 8-15 are in NES 2.0 format
#define NES_2(flag7) ((flag7&0xC)==0x8)

// Mapper Number
#define MAPPER_NUMBER(flag6,flag7) (flag6>>4&(flag7&0xF0))

// TV system (0: NTSC; 1: PAL)
#define PAL_9(flag9) (flag9&0x1))
#define NTSC_9(flag9) (!PAL(flag9))

// TV system (0: NTSC; 2: PAL; 1/3: dual compatible)
#define NTSC_10(flag10) ((flag10&0x3)==0x0)
#define PAL_10(flag10) ((flag10&0x3)==0x2)
#define DUAL_10(flag10) ((flag10&0x3)==0x1||(flag10&0x3)==0x3)
// PRG RAM ($6000-$7FFF) (0: present; 1: not present)
#define RPG_RAM_PRESENT(flag10) (flag10&0x10)
// 0: Board has no bus conflicts; 1: Board has bus conflicts
#define BUS_CONFLICTS(flag10) (flag10&0x20)


#endif