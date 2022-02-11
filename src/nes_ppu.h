#ifndef _NES_PPU_H__
#define _NES_PPU_H__

#include<assert.h>

#include"nes_hardware.h"

// https://wiki.nesdev.org/w/index.php?title=PPU
// https://wiki.nesdev.org/w/index.php?title=PPU_registers
struct NES_PPU_REGISTERS
{
	// $2000
	// NMI enable (V), PPU master/slave (P), sprite height (H), 
	// background tile select (B), sprite tile select (S), increment mode (I), 
	// nametable select (NN)
	ubyte ppu_ctrl;
	// $2001
	// color emphasis (BGR), sprite enable (s), background enable (b), 
	// sprite left column enable (M), background left column enable (m), 
	// greyscale (G)
	ubyte ppu_mask;
	// $2002
	// vblank (V), sprite 0 hit (S), sprite overflow (O); 
	// read resets write pair for $2005/$2006
	ubyte ppu_status;
	// $2003
	// OAM read/write address
	ubyte oam_addr;
	// $2005
	// fine scroll position (two writes: X scroll, Y scroll)
	ubyte ppu_scroll;
	// $2006
	// PPU read/write address (two writes: most significant byte, least significant byte)
	ubyte ppu_addr;
	// $4014
	// OAM DMA high address
	ubyte oam_dma;
};

// https://wiki.nesdev.org/w/index.php?title=PPU_memory_map
struct NES_PPU_INFO
{
	struct NES_PPU_REGISTERS registers;
	// 2kB NES internal VRAM
	ubyte vram[0x800];
	ubyte ext_vram[0x800];
	uword vram_address;

	// 0x400=1KB 16 banks
	// 0-7 PATTERN_TABLE
	// 8-F NAME_TABLE
	ubyte* ppu_banks[0x4000 / 0x400];
	ubyte buffer;

	ubyte palette[0x20];
	ubyte palette_bak[0x20];
	// two_write_flag&0x1==0x0 first write
	// two_write_flag&0x1==0x1 second write
	long double_write_flag;
	// https://wiki.nesdev.org/w/index.php?title=PPU_OAM
	// Object Attribute Memory
	ubyte ppu_oam[0x100];

	p_nes_hardware_info hardware;
};

static const nes_palette_data nes_palette[64] = {
	{0x75, 0x75, 0x75, 0xFF},{0x27, 0x1B, 0x8F, 0xFF},{0x00, 0x00, 0xAB, 0xFF},{0x47, 0x00, 0x9F, 0xFF},
	{0x8F, 0x00, 0x77, 0xFF},{0xAB, 0x00, 0x13, 0xFF},{0xA7, 0x00, 0x00, 0xFF},{0x7F, 0x0B, 0x00, 0xFF},
	{0x43, 0x2F, 0x00, 0xFF},{0x00, 0x47, 0x00, 0xFF},{0x00, 0x51, 0x00, 0xFF},{0x00, 0x3F, 0x17, 0xFF},
	{0x1B, 0x3F, 0x5F, 0xFF},{0x00, 0x00, 0x00, 0xFF},{0x00, 0x00, 0x00, 0xFF},{0x00, 0x00, 0x00, 0xFF},

	{0xBC, 0xBC, 0xBC, 0xFF},{0x00, 0x73, 0xEF, 0xFF},{0x23, 0x3B, 0xEF, 0xFF},{0x83, 0x00, 0xF3, 0xFF},
	{0xBF, 0x00, 0xBF, 0xFF},{0xE7, 0x00, 0x5B, 0xFF},{0xDB, 0x2B, 0x00, 0xFF},{0xCB, 0x4F, 0x0F, 0xFF},
	{0x8B, 0x73, 0x00, 0xFF},{0x00, 0x97, 0x00, 0xFF},{0x00, 0xAB, 0x00, 0xFF},{0x00, 0x93, 0x3B, 0xFF},
	{0x00, 0x83, 0x8B, 0xFF},{0x00, 0x00, 0x00, 0xFF},{0x00, 0x00, 0x00, 0xFF},{0x00, 0x00, 0x00, 0xFF},

	{0xFF, 0xFF, 0xFF, 0xFF},{0x3F, 0xBF, 0xFF, 0xFF},{0x5F, 0x97, 0xFF, 0xFF},{0xA7, 0x8B, 0xFD, 0xFF},
	{0xF7, 0x7B, 0xFF, 0xFF},{0xFF, 0x77, 0xB7, 0xFF},{0xFF, 0x77, 0x63, 0xFF},{0xFF, 0x9B, 0x3B, 0xFF},
	{0xF3, 0xBF, 0x3F, 0xFF},{0x83, 0xD3, 0x13, 0xFF},{0x4F, 0xDF, 0x4B, 0xFF},{0x58, 0xF8, 0x98, 0xFF},
	{0x00, 0xEB, 0xDB, 0xFF},{0x00, 0x00, 0x00, 0xFF},{0x00, 0x00, 0x00, 0xFF},{0x00, 0x00, 0x00, 0xFF},

	{0xFF, 0xFF, 0xFF, 0xFF},{0xAB, 0xE7, 0xFF, 0xFF},{0xC7, 0xD7, 0xFF, 0xFF},{0xD7, 0xCB, 0xFF, 0xFF},
	{0xFF, 0xC7, 0xFF, 0xFF},{0xFF, 0xC7, 0xDB, 0xFF},{0xFF, 0xBF, 0xB3, 0xFF},{0xFF, 0xDB, 0xAB, 0xFF},
	{0xFF, 0xE7, 0xA3, 0xFF},{0xE3, 0xFF, 0xA3, 0xFF},{0xAB, 0xF3, 0xBF, 0xFF},{0xB3, 0xFF, 0xCF, 0xFF},
	{0x9F, 0xFF, 0xF3, 0xFF},{0x00, 0x00, 0x00, 0xFF},{0x00, 0x00, 0x00, 0xFF},{0x00, 0x00, 0x00, 0xFF},
};

//#define PATTERN_TABLE_0 0x0000
//#define PATTERN_TABLE_1 0x1000
//#define NAME_TABLE_0 0x2000
//#define NAME_TABLE_1 0x2400
//#define NAME_TABLE_2 0x2800
//#define NAME_TABLE_3 0x2C00
//#define IMAGE_PALETTE 0x3F00
//#define SPRITE_PALETTE 0x3F10

#define NMI_FLAG 0x80

#define V_BLANK_FLAG 0x80


//After each write to $2007, the address is incremented by either 1 or 32 as dictated by bit 2 of $2000.
#define VRAM_ADDRESS_INCREMENT(ppu_info) \
ppu_info->vram_address += (ppu_info->registers.ppu_ctrl & 0x4 ? 32 : 1)
#define SPRITE_PATTERN(ppu_info) \
(ppu_info->registers.ppu_ctrl & 0x8 ? 4 : 0)
#define IMAGE_PATTERN(ppu_info) \
(ppu_info->registers.ppu_ctrl & 0x10 ? 4 : 0)
#define SPRITE_SIZE(ppu_info) \
(ppu_info->registers.ppu_ctrl & 0x20)
#define GEN_NMI(ppu_info) \
(ppu_info->registers.ppu_ctrl&NMI_FLAG)
#define SET_B_BLANK(ppu_info) \
(ppu_info->registers.ppu_status |= V_BLANK_FLAG)
#define RST_B_BLANK(ppu_info) \
(ppu_info->registers.ppu_status &= (~V_BLANK_FLAG))

static inline ubyte read_ppu_register_via_cpu(p_nes_ppu_info info, uword address)
{
	ubyte result = address >> 8;

	if ((address & 0x7) == 0x7)
	{
		// The first read from $2007 is invalid and the data will actually be buffered and
		// returned on the next read.This does not apply to colour palettes.

		uword addr = info->vram_address;
		VRAM_ADDRESS_INCREMENT(info);

		addr &= 0x3FFF;
		result = info->buffer;
		// >>10 == addr/0x400 addr & 0x3FF == add%0x400
		info->buffer = info->ppu_banks[addr >> 10][addr & 0x3FF];

		// name tables
		if (addr >= 0x3F00)
			result = info->palette[addr & 0x1F];

		return result;
	}
	else if ((address & 0x7) == 0x4)
	{
		// reads during vertical or forced blanking return the value from OAM at that address but do not increment.
		return info->ppu_oam[info->registers.oam_addr & 0xFF];
	}
	else if ((address & 0x7) == 0x2)
	{
		// When a read from $2002 occurs, bit 7 is reset to 0 as are $2005 and $2006.

		result = info->registers.ppu_status;

		RST_B_BLANK(info);
		info->double_write_flag = 0;

		return result;
	}

	return result;
}

static inline void write_ppu_register_via_cpu(p_nes_ppu_info info, uword address, ubyte data)
{
	switch (address & 0x7)
	{
	case 0x0:
		info->registers.ppu_ctrl = data;
		break;
	case 0x1:
		info->registers.ppu_mask = data;
		break;
	case 0x3:
		info->registers.oam_addr = data;
		break;
	case 0x4:
		// Write OAM data here. Writes will increment OAMADDR after the write; 
		info->ppu_oam[info->registers.oam_addr & 0xFF] = data;
		info->registers.oam_addr++;
		break;
	case 0x5:
		// ÔÝÊ±Ã»ÓÃ
		info->double_write_flag++;
		break;
	case 0x6:
		if (info->double_write_flag & 0x1)
			// upper address
			info->vram_address = (info->vram_address & 0xFF00) | ((uword)data & 0xFF);
		else
			info->vram_address = (info->vram_address & 0xFF) | ((uword)data << 8);

		info->double_write_flag++;
		break;
	case 0x7:
	{
		uword addr = info->vram_address;
		VRAM_ADDRESS_INCREMENT(info);

		addr &= 0x3FFF;
		if (addr < 0x3F00)
		{
			assert(addr >= 0x2000);
			info->ppu_banks[addr >> 10][addr & 0x3FF] = data;
		}
		else
		{
			if (addr & 0x3)
				info->palette[addr & 0x1F] = data;
			else
			{
				// Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C. 
				info->palette[addr & 0xF] = data;
				info->palette[addr & 0xF | 0x10] = data;
			}
		}
	}
	break;
	}
}

void init_ppu(p_nes_ppu_info info);
void reset_ppu(p_nes_ppu_info info);

#endif