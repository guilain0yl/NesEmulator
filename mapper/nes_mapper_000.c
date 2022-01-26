#include"../nes_hardware.h"

// https://wiki.nesdev.org/w/index.php?title=NROM
int nes_mapper_000_reset(p_nes_hardware_info info)
{
	if (info->rom_info->rom_header->prg_size < 1 || info->rom_info->rom_header->prg_size>2)
		return NES_MAPPER_PRG_COUNT_ERROR;

	info->mem_info->current_lower_bank_pointer = info->rom_info->prg_data;
	info->mem_info->current_upper_bank_pointer = info->rom_info->rom_header->prg_size == 1
		? info->rom_info->prg_data
		: info->rom_info->prg_data + _16_KB_;

	// load chr
	if (info->rom_info->rom_header->chr_size > 0)
	{
		info->ppu_info->pattern_table_0 = info->rom_info->chr_data;
		info->ppu_info->pattern_table_1 = info->rom_info->chr_data + _4_KB_;
	}

	return NES_SUCCESS;
}