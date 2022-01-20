#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"nes_rom.h"


static int malloc_and_read(FILE* fp, void** ptr, size_t size, size_t count)
{
	*ptr = malloc(size * count);
	if (*ptr == NULL)
		return NES_MEMORRY_ERROR;

	memset(*ptr, 0x0, size * count);
	if (fread(*ptr, size, count, fp) <= 0)
		return NES_ROM_READ_ERROR;

	return NES_SUCCESS;
}

int load_rom(const char* path, p_nes_rom_info info)
{
	FILE* fp = NULL;
	int result = NES_SUCCESS;

	if (info == NULL)
	{
		result = NES_NULL_POINT_ERROR;
		goto end;
	}

	fp = fopen(path, "rb");
	if (fp == NULL)
	{
		result = NES_ROM_NOT_FOUND_ERROR;
		goto end;
	}

	if (info->rom_header == NULL)
		info->rom_header = (p_rom_header)malloc(sizeof(rom_header));

	if (info->rom_header == NULL)
	{
		result = NES_MEMORRY_ERROR;
		goto end;
	}

	memset(info->rom_header, 0x0, sizeof(rom_header));

	if (fread(info->rom_header, sizeof(rom_header), 1, fp) <= 0)
	{
		result = NES_ROM_READ_ERROR;
		goto end;
	}

	// check nes format
	if (!(info->rom_header->constant[0] == 0x4E &&
		info->rom_header->constant[1] == 0x45 &&
		info->rom_header->constant[2] == 0x53 &&
		info->rom_header->constant[3] == 0x1A))
	{
		result = NES_ROM_FORMAT_UNSUPPORT_ERROR;
		goto end;
	}

	info->mapper_number = MAPPER_NUMBER(info->rom_header->flag_6, info->rom_header->flag_7);
	if (NES_2(info->rom_header->flag_7))
	{
		result = NES_ROM_VERSION_UNSUPPORT_ERROR;
		goto end;
	}

	// load Trainer
	if (TRAINER(info->rom_header->flag_6))
	{
		result = malloc_and_read(fp, &info->trainer_data, 512, 1);
		if (result != NES_SUCCESS)
			goto end;
	}

	// load prg rom
	result = malloc_and_read(fp, &info->prg_data, _16_KB_, info->rom_header->prg_size);
	if (result != NES_SUCCESS)
		goto end;

	// load chr rom
	if (info->rom_header->chr_size > 0)
	{
		result = malloc_and_read(fp, &info->chr_data, _8_KB_, info->rom_header->chr_size);
		if (result != NES_SUCCESS)
			goto end;
	}

end:
	if (fp != NULL)
		fclose(fp);

	return result;
}

int unload_rom(p_nes_rom_info info)
{
	if (info->rom_header != NULL)
		free(info->rom_header);

	if (info->trainer_data != NULL)
		free(info->trainer_data);

	if (info->prg_data != NULL)
		free(info->prg_data);

	if (info->chr_data != NULL)
		free(info->chr_data);

	memset(info, 0x0, sizeof(nes_rom_info));

	return NES_SUCCESS;
}