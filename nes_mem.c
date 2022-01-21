#include"nes_hardware.h"

static inline byte zp_read_byte(p_nes_mem_info info, ubyte address)
{
	return info->memory[address];
}

static inline void zp_write_byte(p_nes_mem_info info, ubyte address, byte data)
{
	info->memory[address] = data;
}

static inline word zp_read_word(p_nes_mem_info info, ubyte address)
{
	return info->memory[address] | (word)info->memory[address + 1] << 8;
}

static inline byte read_byte(p_nes_mem_info info, uword address)
{
	switch (address & 0xE000)
	{
	case RAM_START://RAM
		return info->memory[address & 0x7FF];
	case RAM_PPU://PPU
		break;
	case RAM_SOUND://SOUND
		break;
	case RAM_SRAM:
		break;
	case RAM_PRG_LOWER_BANK:
		break;
	case RAM_PRG_UPPER_BANK:
		break;
	}

	return address >> 8;
}

static inline void write_byte(p_nes_mem_info info, uword address, byte data)
{
	switch (address & 0xE000)
	{
	case RAM_START://RAM
		info->memory[address & 0x7FF] = data;
	case RAM_PPU://PPU
		break;
	case RAM_SOUND://SOUND
		break;
	case RAM_SRAM:
		break;
	case RAM_PRG_LOWER_BANK:
		break;
	case RAM_PRG_UPPER_BANK:
		break;
	}
}

static inline word read_word(p_nes_mem_info info, uword address)
{
	return 0x00FF == (address & 0x00FF)
		? read_byte(info, address) | (word)read_byte(info, address - 0x00FF) << 8
		: read_byte(info, address) | read_byte(info, address + 1) << 8;
}

static inline void write_word(p_nes_mem_info info, uword address, word data)
{
	write_byte(info, address, data & 0xFF);
	write_byte(info, address + 1, data >> 8);
}