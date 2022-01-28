#include<string.h>

#include"nes_rom.h"
#include"nes_ppu.h"


void init_ppu(p_nes_ppu_info info)
{

}

static void init_ppu_banks(p_nes_ppu_info info)
{
	p_nes_rom_info p_rom_info = info->hardware->p_rom_info;

	if (FOUR_SCREEN_VRAM(p_rom_info->rom_header.flag_6))
	{
		info->ppu_banks[0x8] = info->vram;
		info->ppu_banks[0x9] = info->vram + 0x400;
		info->ppu_banks[0xA] = info->ext_vram;
		info->ppu_banks[0xB] = info->ext_vram + 0x400;
	}
	else if (MIRROR_MODE_VERTICAL(p_rom_info->rom_header.flag_6))
	{
		info->ppu_banks[0x8] = info->vram;
		info->ppu_banks[0x9] = info->vram + 0x400;
		info->ppu_banks[0xA] = info->vram;
		info->ppu_banks[0xB] = info->vram + 0x400;
	}
	else if (MIRROR_MODE_HORIZONTAL(p_rom_info->rom_header.flag_6))
	{
		info->ppu_banks[0x8] = info->vram;
		info->ppu_banks[0x9] = info->vram;
		info->ppu_banks[0xA] = info->vram + 0x400;
		info->ppu_banks[0xB] = info->vram + 0x400;
	}

	// mirror 0x2000-0x2EFF
	info->ppu_banks[0xC] = info->ppu_banks[0x8];
	info->ppu_banks[0xD] = info->ppu_banks[0x9];
	info->ppu_banks[0xE] = info->ppu_banks[0xA];
	info->ppu_banks[0xF] = info->ppu_banks[0xB];
}

void reset_ppu(p_nes_ppu_info info)
{
	p_nes_hardware_info ptr = info->hardware;
	memset(info, 0x0, sizeof(nes_ppu_info));
	info->hardware = ptr;

	init_ppu_banks(info);
}

static inline void get_pixel(p_nes_ppu_info info, int count,
	ubyte* bg_name_table,
	ubyte* bg_pattern_table,
	p_nes_palette_data data)
{
	ubyte* attribute_table = bg_name_table + (32 * 30);
	ubyte* tile = bg_name_table + count;
	ubyte* l = bg_pattern_table + (*tile * 16);
	ubyte* h = l + 8;

	count++;

	int r = (count >> 5) >> 2;
	int c = (count & 0x1F) >> 2;

	int ri = (count >> 5) & 0x3;
	int ci = (count & 0x1F) & 0x3;

	ubyte attribute = *(attribute_table + (r * 8) + c);

	ubyte hb = attribute >> ((ri > 1) ? (ci < 3 ? 4 : 6) : (ci < 3 ? 0 : 2));
	hb = (hb << 2) & 0xC;

	for (size_t i = 0; i < 8; i++)
	{
		data[0 + (i << 8)] = nes_palette[info->palette_bak[hb | ((h[i] & 0x80) >> 6) | ((l[i] & 0x80) >> 7)] & 0x3F];
		data[1 + (i << 8)] = nes_palette[info->palette_bak[hb | ((h[i] & 0x40) >> 5) | ((l[i] & 0x40) >> 6)] & 0x3F];
		data[2 + (i << 8)] = nes_palette[info->palette_bak[hb | ((h[i] & 0x20) >> 4) | ((l[i] & 0x20) >> 5)] & 0x3F];
		data[3 + (i << 8)] = nes_palette[info->palette_bak[hb | ((h[i] & 0x10) >> 3) | ((l[i] & 0x10) >> 4)] & 0x3F];
		data[4 + (i << 8)] = nes_palette[info->palette_bak[hb | ((h[i] & 0x8) >> 2) | ((l[i] & 0x8) > 3)] & 0x3F];
		data[5 + (i << 8)] = nes_palette[info->palette_bak[hb | ((h[i] & 0x4) >> 1) | ((l[i] & 0x4) > 2)] & 0x3F];
		data[6 + (i << 8)] = nes_palette[info->palette_bak[hb | (h[i] & 0x2) | ((l[i] & 0x2) > 1)] & 0x3F];
		data[7 + (i << 8)] = nes_palette[info->palette_bak[hb | ((h[i] & 0x1) << 1) | (l[i] & 0x1)] & 0x3F];
	}
}

int render(p_nes_ppu_info info, void* data)
{
	p_nes_palette_data ptr = (p_nes_palette_data)data;

	if (ptr == NULL)
		return NES_NULL_POINT_ERROR;

	memset(data, 0x0, 256 * 240 * sizeof(nes_palette_data));

	for (size_t i = 0; i < 32; i++)
		info->palette_bak[i] = info->palette[i];

	info->palette_bak[0x4] = info->palette_bak[0x8] = info->palette_bak[0xC] =
		info->palette_bak[0x10] = info->palette_bak[0x14] = info->palette_bak[0x18] =
		info->palette_bak[0x1C] = info->palette_bak[0];

	ubyte* bg_name_table = info->ppu_banks[8];
	ubyte* bg_pattern_table = info->ppu_banks[IMAGE_PATTERN(info)];

	for (size_t i = 0; i < 32 * 30; i++)
	{
		//get_pixel(info, i, bg_name_table, bg_pattern_table, ptr + ((i >> 5) << 5 << 3 << 3) + ((i & 0x1F) << 3));

		get_pixel(info, i, bg_name_table, bg_pattern_table, ptr + ((i / 32) * 32 * 8 * 8) + ((i & 0x1F) * 8));
	}

	return NES_SUCCESS;
}