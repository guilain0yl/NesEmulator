#include"../nes_mapper.h"

int nes_mapper_000_reset(p_nes_hardware_info info)
{
	if (info->rom_info->rom_header->prg_size == 1)
	{
		info->mem_info->current_lower_bank_pointer = info->rom_info->prg_data;
		info->mem_info->current_upper_bank_pointer = info->rom_info->prg_data;
		return NES_SUCCESS;
	}
	else if (info->rom_info->rom_header->prg_size == 2)
	{
		info->mem_info->current_lower_bank_pointer = info->rom_info->prg_data;
		info->mem_info->current_upper_bank_pointer = (void*)((int)(info->rom_info->prg_data) + _16_KB_);
		return NES_SUCCESS;
	}
	else
	{
		return NES_MAPPER_PRG_COUNT_ERROR;
	}
}