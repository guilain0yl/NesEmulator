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

ubyte read_byte_via_cpu(void* hardware, uword address)
{
	p_nes_ppu_info info = ((p_nes_hardware_info)hardware)->ppu_info;

	if ((address & 0x7) == 0x7)
	{
		uword real_address = address & 0x3FFF;

		if (real_address < 0x3F00)
		{
			ubyte data = info->buffer;

			if (real_address >= 0x3000)
				real_address &= 0x2FFF;

			if (real_address < 0x1000)
				info->buffer = info->pattern_table_0[real_address];
			else if (real_address < 0x2000)
				info->buffer = info->pattern_table_0[real_address];
			else if (real_address < 0x2800)
				info->buffer = info->name_attribute_tables[real_address - 0x2000];
			else if (real_address < 0x3000)
				info->buffer = 0x0;

			return data;
		}
		else
		{
			return info->buffer = info->palette[real_address & (ubyte)0x1f];
		}
	}
	else if ((address & 0x7) == 0x4)
	{
		return info->sprites[info->sprites_address++];
	}
	else if ((address & 0x7) == 0x2)
	{
		ubyte data = *info->registers.ppu_status_register;
		*info->registers.ppu_status_register &= ~0x80;
		return data;
	}

	return (address >> 8);
}
void write_byte_via_cpu(void* hardware, uword address, ubyte data)
{
	p_nes_ppu_info info = ((p_nes_hardware_info)hardware)->ppu_info;

	switch (address & 0x7)
	{
	case 0:
		*info->registers.ppu_control_register_1 = data;
		break;
	case 1:
		*info->registers.ppu_control_register_2 = data;
		break;
	case 3:
		info->sprites_address = data;
		break;
	case 4:
		info->sprites[info->sprites_address++] = data;
		break;
	case 5:
		info->scroll[info->writex2 & 1] = data;
		info->writex2++;
		break;
	case 6:
		if (info->writex2 & 1)
			info->vramaddr = (info->vramaddr & 0xFF00) | data;
		else
			info->vramaddr = (info->vramaddr & 0x00FF) | ((uword)data << 8);
		info->writex2++;
		break;
	case 7:
	{
		uword real_address = address & 0x3FFF;
		if (real_address < 0x3F00)
		{
			ubyte data = info->buffer;

			if (real_address >= 0x3000)
				real_address &= 0x2FFF;

			if (real_address >= 0x2000)
				info->name_attribute_tables[real_address - 0x2000] = data;
			else if (real_address >= 0x2800)
				;
		}
		else
		{
			if (real_address & 0x03)
				info->palette[real_address & 0x1f] = data;
			else
			{
				const uword offset = real_address & 0x0f;
				info->palette[offset] = data;
				info->palette[offset | 0x10] = data;
			}
		}
		info->vramaddr += (*info->registers.ppu_control_register_1) & 0x4 ? 32 : 1;
	}
	break;
	}
}

void vblank_flag_start(p_nes_ppu_info info) {
	*info->registers.ppu_status_register |= 0x80;
}

void vblank_flag_end(p_nes_ppu_info info) {
	*info->registers.ppu_status_register &= ~0x80;
}