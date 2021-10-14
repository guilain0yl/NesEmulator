#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"nes_file.h"

static inline int nes_read(FILE* fp, const void** data, size_t element_size, size_t element_count)
{
	int result = 0;

	*data = malloc(element_size * element_count);
	if ((*data) == NULL)
		return -1;

	result = fread((*data), element_size, element_count, fp);
	if (result != element_count)
		return -1;

	return 0;
}

int load(const char* file_path, const p_nes_info info)
{
	FILE* fp = NULL;
	int result = 0;

	fp = fopen(file_path, "rb");
	if (fp == NULL)
		return -1;

	// load nes header
	if (nes_read(fp, &info->nes_header, sizeof(nes_header), 1) < 0)
		goto ERROR;

	if (!(info->nes_header->head[0] == 'N' && info->nes_header->head[1] == 'E' && info->nes_header->head[2] == 'S' && info->nes_header->head[3] == 0x1A))
		goto ERROR;

	if (NES_2(info->nes_header->flag_7))
		goto ERROR;

	// load trainer data

	if (TRAINER(info->nes_header->flag_6))
	{
		if (nes_read(fp, &info->trainer_data, 512, 1) < 0)
			goto ERROR;
	}

	// load prg rom data
	if (nes_read(fp, &info->prg_data, PRG_UNIT_SIZE, info->nes_header->prg_rom_size) < 0)
		goto ERROR;

	// load chr rom data
	if (nes_read(fp, &info->chr_data, CHR_UNIT_SIZE, info->nes_header->chr_rom_size) < 0)
		goto ERROR;

	fclose(fp);
	return 0;

ERROR:
	fclose(fp);
	return -1;
}

static inline void reset(const void** pointer)
{
	if (*pointer != NULL)
	{
		free(*pointer);
		*pointer = NULL;
	}
}

void unload(const p_nes_info info)
{
	reset(&info->nes_header);
	reset(&info->prg_data);
	reset(&info->chr_data);
	reset(&info->trainer_data);
}

int main()
{


	nes_info info;
	info.chr_data = NULL;
	info.nes_header = NULL;
	info.prg_data = NULL;
	info.trainer_data = NULL;
	unsigned char s = ~(1 >> 0);

	load("F:\\m_project\\NesEmulator\\readme\\nestest.nes", &info);
	// unload(&info);

	return 0;
}