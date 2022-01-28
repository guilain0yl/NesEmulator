#include"../nes_rom.h"
#include"../nes_cpu.h"
#include"../nes_mem.h"

// https://wiki.nesdev.org/w/index.php?title=Mapper
// https://wiki.nesdev.org/w/index.php?title=NROM
int nes_mapper_000_reset(p_nes_hardware_info info)
{
	if (info->p_rom_info->rom_header.prg_size != 1 && info->p_rom_info->rom_header.prg_size != 2)
		return NES_MAPPER_PRG_COUNT_ERROR;

	info->p_cpu_info->memory->current_lower_bank_pointer = info->p_rom_info->prg_data;
	info->p_cpu_info->memory->current_upper_bank_pointer = info->p_rom_info->rom_header.prg_size == 1
		? info->p_rom_info->prg_data
		: info->p_rom_info->prg_data + _16_KB_;

	// load chr
	if (info->p_rom_info->rom_header.chr_size == 1)
	{
		for (int i = 0; i < 8; i++)
			info->p_ppu_info->ppu_banks[i] = info->p_rom_info->chr_data + i * __KB__;
	}

	return NES_SUCCESS;
}