#include"../nes_rom.h"
#include"../nes_cpu.h"
#include"../nes_mem.h"


int nes_mapper_000_reset(p_nes_hardware_info info)
{
	if (info->p_rom_info->rom_header.prg_size < 1 || info->p_rom_info->rom_header.prg_size>2)
		return NES_MAPPER_PRG_COUNT_ERROR;

	info->p_cpu_info->memory->current_lower_bank_pointer = info->p_rom_info->prg_data;
	info->p_cpu_info->memory->current_upper_bank_pointer = info->p_rom_info->rom_header.prg_size == 1
		? info->p_rom_info->prg_data
		: info->p_rom_info->prg_data + _16_KB_;

	// load chr
	if (info->p_rom_info->rom_header.chr_size > 0)
	{
		/*info->ppu_info->pattern_table_0 = info->rom_info->chr_data;
		info->ppu_info->pattern_table_1 = info->rom_info->chr_data + _4_KB_;*/
	}

	return NES_SUCCESS;
}