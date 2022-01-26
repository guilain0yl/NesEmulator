#include<stdlib.h>
#include<string.h>

#include"nes_hardware.h"

void init_ppu(p_nes_ppu_info info)
{
	info->registers.ppu_control_register_1 =
		&((p_nes_hardware_info)info->hardware)->mem_info->ppu_registers[0];
	info->registers.ppu_control_register_2 =
		&((p_nes_hardware_info)info->hardware)->mem_info->ppu_registers[1];
	info->registers.ppu_status_register =
		&((p_nes_hardware_info)info->hardware)->mem_info->ppu_registers[2];
	info->registers.spr_ram_address_register =
		&((p_nes_hardware_info)info->hardware)->mem_info->ppu_registers[3];
	info->registers.spr_ram_io_register =
		&((p_nes_hardware_info)info->hardware)->mem_info->ppu_registers[4];
	info->registers.vram_address_register_1 =
		&((p_nes_hardware_info)info->hardware)->mem_info->ppu_registers[5];
	info->registers.vram_address_register_2 =
		&((p_nes_hardware_info)info->hardware)->mem_info->ppu_registers[6];
	info->registers.vram_io_register =
		&((p_nes_hardware_info)info->hardware)->mem_info->ppu_registers[7];
}

void reset_ppu(p_nes_ppu_info info)
{
	info->pattern_table_0 = NULL;
	info->pattern_table_1 = NULL;

	memset(info->name_attribute_tables, 0x0, sizeof(info->name_attribute_tables));
	if (info->ext_name_attribute_tables != NULL)
	{
		free(info->ext_name_attribute_tables);
		info->ext_name_attribute_tables = NULL;
	}

	memset(info->palette, 0x0, sizeof(info->palette));
}