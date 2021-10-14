#ifndef __NES_H_
#define __NES_H_

#define _KB_ 1024
#define PRG_UNIT_SIZE (_KB_*16)
#define CHR_UNIT_SIZE (_KB_*8)

// https://wiki.nesdev.org/w/index.php?title=INES
typedef struct NES_HEADER
{
	char head[4];
	// Size of PRG ROM in 16 KB units
	unsigned char prg_rom_size;
	// Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
	unsigned char chr_rom_size;
	unsigned char flag_6;
	unsigned char flag_7;
	// PRG RAM size
	unsigned char flag_8;
	// (0000 0001) TV system (0: NTSC; 1: PAL)
	unsigned char flag_9;
	unsigned char flag_10;
	unsigned char unused_ext[5];
}nes_header, * p_nes_header;

typedef struct NES_INFO
{
	p_nes_header nes_header;
	void* prg_data;
	void* chr_data;
	void* trainer_data;
}nes_info, * p_nes_info;


// 0: horizontal (vertical arrangement) (CIRAM A10 = PPU A11)
// 1: vertical (horizontal arrangement) (CIRAM A10 = PPU A10)
#define MIRROR_MODE(flag6) ((flag6&0x1)==0x1)
// 1: Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory
#define EXT_MEMORY(flag6) ((flag6&0x2)==0x2)
// 1: 512-byte trainer at $7000-$71FF (stored before PRG data)
#define TRAINER(flag6) ((flag6&0x4)==0x4)
// 1: Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM
#define FOUR_SCREEN_VRAM(flag6) ((flag6&0x8)==0x8)

// VS Unisystem
#define VS_UNISYSTEM(flag7) ((flag7&0x1)==0x1)
// PlayChoice-10 (8KB of Hint Screen data stored after CHR data)
#define PLAY_CHOICE(flag7) ((flag7&0x2)==0x2)
// If equal to 2, flags 8-15 are in NES 2.0 format
#define NES_2(flag7) ((flag7&0xC)==0x8)

// Mapper Number
#define MAPPER_NUMBER(flag6,flag7) ((unsigned char)(((flag6&0xF0)>>4)&(flag7&0xF0)))

// TV system (0: NTSC; 1: PAL)
#define TV_SYSTEM(flag9) ((flag9&0x1)==0x1)

// TV system (0: NTSC; 2: PAL; 1/3: dual compatible)
#define TV_SYSTEM_EXT(flag10) (flag10&0x3)

// PRG RAM ($6000-$7FFF) (0: present; 1: not present)
#define RPG_RAM(flag10) ((flag10&0x10)==0x10)

// 0: Board has no bus conflicts; 1: Board has bus conflicts
#define RPG_RAM(flag10) ((flag10&0x20)==0x20)

int load(const char* file_path, const p_nes_info info);
void unload(const p_nes_info info);

#endif