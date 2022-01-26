#ifndef _NES_PPU_H__
#define _NES_PPU_H__

#include"nes_utils.h"

// https://wiki.nesdev.org/w/index.php?title=PPU_registers
// todo: rename
typedef struct NES_PPU_REGISTERS
{
	// $2000
	// PPU Control Register 1
	ubyte* ppu_control_register_1;
	// $2001
	// PPU Control Register 2
	ubyte* ppu_control_register_2;
	// $2002
	// PPU Status Register
	ubyte* ppu_status_register;
	// $2003
	// SPR-RAM Address Register
	// Holds the address in SPR-RAM to access on the next write to $2004
	ubyte* spr_ram_address_register;
	// $2004
	// SPR-RAM I/O Register
	// Writes a byte to SPR-RAM at the address indicated by $2003
	ubyte* spr_ram_io_register;
	// $2005
	// VRAM Address Register 1
	ubyte* vram_address_register_1;
	// $2006
	// VRAM Address Register 2
	ubyte* vram_address_register_2;
	// $2007
	// VRAM I/O Register
	// Reads or writes a byte from VRAM at the current address
	ubyte* vram_io_register;
}nes_ppu_registers;

typedef struct NES_PPU_INFO
{
	// pattern tables
	ubyte* pattern_table_0;
	ubyte* pattern_table_1;

	// Name tables and Attribute tables
	ubyte name_attribute_tables[0x800];
	ubyte* ext_name_attribute_tables;

	// palette index
	ubyte palette[0x20];
	nes_ppu_registers registers;

	// buffer value
	ubyte buffer;

	// The PPU also has a separate 256 byte area of memory, SPR-RAM (Sprite RAM), 
	// to store the sprite attributes.
	ubyte sprites[0x100];
	ubyte sprites_address;

	ubyte scroll[2];
	ubyte writex2;
	uword vramaddr;
	void* hardware;
}nes_ppu_info, * p_nes_ppu_info;

static const union nes_palette_data {
	struct { ubyte r, g, b, a; };
	unsigned int data;
}nes_palette[64] = {
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

ubyte read_byte_via_cpu(void* hardware, uword address);
void write_byte_via_cpu(void* hardware, uword address, ubyte data);

void init_ppu(p_nes_ppu_info info);
void reset_ppu(p_nes_ppu_info info);

void vblank_flag_start(p_nes_ppu_info info);
void vblank_flag_end(p_nes_ppu_info info);

#endif // !_NES_PPU_H__
