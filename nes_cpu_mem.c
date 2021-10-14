#include"nes_cpu.h"
#include"nes_cpu_mem.h"

byte zp_read_byte(p_cpu_info info, byte address)
{
	return info->memory[address];
}

void zp_write_byte(p_cpu_info info, byte address, byte value)
{
	info->memory[address] = value;
}

word zp_read_word(p_cpu_info info, byte address)
{
	return zp_read_byte(info, address) | (((word)zp_read_byte(info, address + 1)) << 8);
}

byte read_byte(p_cpu_info info, word address)
{
	switch (address & 0xe000)
	{
		// RAM
	case RAM_START:
		// 0x0000 - 0x1fff  RAM(0x800 - 0x1fff is mirror of 0x0 - 0x7ff)
		return info->memory[address & 0x1ff];
	case PPU_REGISTERS:
		break;
		// sound
	case IO_REGISTERS:
		break;
		// SRAM
	case SRAM:
		break;
		// PRG ROM
	case PRG_ROM_L:
		break;
	case PRG_ROM_H:
		break;
	}

	// when a register is not readable the upper half address is returned.
	return (address >> 8);
}

void write_byte(p_cpu_info info, word address, byte value)
{
	switch (address & 0xe000)
	{
		// RAM
	case RAM_START:
		//0x0000 - 0x1fff  RAM ( 0x800 - 0x1fff is mirror of 0x0 - 0x7ff )
		info->memory[address & 0x7ff] = value;
	case PPU_REGISTERS:
		break;
		// sound
	case IO_REGISTERS:
		break;
		// SRAM
	case SRAM:
		break;
		// PRG ROM
	case PRG_ROM_L:
		break;
	case PRG_ROM_H:
		break;
	}
}

word read_word(p_cpu_info info, word address)
{
	return read_byte(info, address) | (((word)read_byte(info, address + 1)) << 8);
}

void write_word(p_cpu_info info, word address, word value)
{
	write_byte(info, address, (byte)(value & 0xFF));
	write_byte(info, address + 1, (byte)((value >> 8) & 0xFF));
}

void push_byte(p_cpu_info info, byte value)
{
	write_byte(info, BASE_STACK + info->registers.SP--, value);
}

byte pop_byte(p_cpu_info info)
{
	return read_byte(info, BASE_STACK + info->registers.SP++);
}

void push_word(p_cpu_info info, word value)
{
	push_byte(info, (byte)(value >> 8));
	push_byte(info, (byte)(value & 0xFF));
}

word pop_word(p_cpu_info info)
{
	byte l = pop_byte(info);
	byte h = pop_byte(info);
	return ((word)h << 8) & l;
}