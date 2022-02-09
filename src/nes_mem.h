#ifndef _NES_MEM_H__
#define _NES_MEM_H__

#include"nes_hardware.h"
#include"nes_ppu.h"
#include"nes_cpu.h"
#include"nes_apu.h"

struct NES_JOYPAD_INFO
{
	ubyte joypad, joypad_index;
};

struct NES_MEM_INFO
{
	// 2KB physical memory
	ubyte memory[0x800];
	ubyte sram[0x2000];
	struct NES_JOYPAD_INFO joypad1, joypad2;
	ubyte joypad_read_flag;
	void* current_lower_bank_pointer;
	void* current_upper_bank_pointer;
	p_nes_cpu_info p_cpu_info;
};

#define RAM_START 0x0000
#define BASE_STACK 0x100
#define RAM_PPU 0x2000
#define RAM_SOUND 0x4000
#define EXT_ROM 0x4020
#define RAM_SRAM 0x6000
#define ROM_PRG_LOWER_BANK 0x8000
#define ROM_PRG_LOWER_BANK_2 0xA000
#define ROM_PRG_UPPER_BANK 0xC000
#define ROM_PRG_UPPER_BANK_2 0xE000

static inline ubyte zp_read_byte(p_nes_mem_info info, ubyte address)
{
	return info->memory[address];
}

static inline void zp_write_byte(p_nes_mem_info info, ubyte address, ubyte data)
{
	info->memory[address] = data;
}

static inline uword zp_read_word(p_nes_mem_info info, ubyte address)
{
	return zp_read_byte(info, address) | ((uword)zp_read_byte(info, address + 1)) << 8;
}

static inline ubyte read_byte(p_nes_mem_info info, uword address)
{
	switch (address & 0xE000)
	{
	case RAM_START://RAM 2kb physical memory
		// 0x800-0x1FFF mirrors(0X0-0X7FF)
		return info->memory[address & 0x7FF];
	case RAM_PPU://PPU
		return read_ppu_register_via_cpu(info->p_cpu_info->hardware->p_ppu_info, address);
	case RAM_SOUND://SOUND
		return read_apu_byte(info, address);
	case RAM_SRAM:
		break;
	case ROM_PRG_LOWER_BANK:
	case ROM_PRG_LOWER_BANK_2:
		return ((ubyte*)info->current_lower_bank_pointer)[address & 0x7FFF];
	case ROM_PRG_UPPER_BANK:
	case ROM_PRG_UPPER_BANK_2:
		return ((ubyte*)info->current_upper_bank_pointer)[address & 0x3FFF];
	}

	return address >> 8;
}

static inline void write_byte(p_nes_mem_info info, uword address, ubyte data)
{
	switch (address & 0xE000)
	{
	case RAM_START://RAM
		info->memory[address & 0x7FF] = data;
		break;
	case RAM_PPU://PPU
		write_ppu_register_via_cpu(info->p_cpu_info->hardware->p_ppu_info, address, data);
		break;
	case RAM_SOUND://SOUND
		write_apu_byte(info, address, data);
		break;
	case RAM_SRAM:
		break;
	}
}

static inline uword read_word(p_nes_mem_info info, uword address)
{
	return read_byte(info, address) | ((uword)read_byte(info, address + 1)) << 8;
}

// 6502's indirect absolute jmp(opcode: 6C) has a bug (added at 01/08/15 )
static inline uword read_word_jump_ind(p_nes_mem_info info, uword address)
{
	return 0x00FF == (address & 0x00FF)
		? read_byte(info, address) | ((uword)read_byte(info, address - 0x00FF)) << 8
		: read_byte(info, address) | ((uword)read_byte(info, address + 1)) << 8;
}

static inline void write_word(p_nes_mem_info info, uword address, uword data)
{
	write_byte(info, address, data & 0xFF);
	write_byte(info, address + 1, data >> 8);
}

#endif
