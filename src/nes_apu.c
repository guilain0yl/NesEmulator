#include<string.h>

#include"nes_apu.h"
#include"nes_mem.h"

ubyte read_apu_byte(p_nes_mem_info info, uword address)
{
	ubyte data = address >> 8;

	switch (address & 0x1F)
	{
	case 0x16:
		// joypad 1
		data = info->joypad_read_flag
			? (info->joypad1.joypad & 0x1)
			: ((info->joypad1.joypad >> (info->joypad1.joypad_index & 0x7)) & 0x1);
		info->joypad1.joypad_index++;
		break;
	case 0x17:
		// joypad 2
		data = info->joypad_read_flag
			? (info->joypad2.joypad & 0x1)
			: ((info->joypad2.joypad >> (info->joypad2.joypad_index & 0x7)) & 0x1);
		info->joypad2.joypad_index++;
		break;
	}

	return data;
}

static inline void* get_dma_address(p_nes_mem_info info, ubyte data)
{
	// This port is located on the CPU. Writing $XX will upload 256 bytes of data from CPU page $XX00-$XXFF to the internal PPU OAM. 
	// This page is typically located in internal RAM, commonly $0200-$02FF, but cartridge RAM or ROM can be used as well.
	switch (data >> 5)
	{
	case 0x0:
		// RAM
		return info->memory + ((((uword)data) << 8) & 0x7FF);
	case 0x2:
		//
	case 0x3:
		// SRAM
		return info->sram + ((((uword)data) << 8) & 0x1FFF);
		break;
	case 0x4:
		// 0x8000-0xBFFF
		//RAM_PRG_LOWER_BANK
		return (ubyte*)info->current_lower_bank_pointer + ((((uword)data) << 8) & 0x7FFF);
	case 0x6:
		//RAM_PRG_UPPER_BANK
		return (ubyte*)info->current_upper_bank_pointer + ((((uword)data) << 8) & 0x3FFF);
	}

	return 0;
}

void write_apu_byte(p_nes_mem_info info, uword address, ubyte data)
{
	switch (address & 0x1F)
	{
	case 0x14:
		memcpy(info->p_cpu_info->hardware->p_ppu_info->ppu_oam, get_dma_address(info, data), 0x100);
		break;
	case 0x16:
		// joypad 1
		info->joypad_read_flag = data & 0x1;
		if (info->joypad_read_flag)
		{
			info->joypad1.joypad_index = 0;
			info->joypad2.joypad_index = 0;
		}
		break;
	case 0x17:
		break;
	}
}

