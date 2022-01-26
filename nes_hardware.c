#include<stdlib.h>
#include<string.h>

#include"nes_hardware.h"

#define NES_CASE_LOAD_MAPPER(id,info) case 0x##id: result=nes_mapper_##id##_reset(info);break;

// init_hardware->load_rom->execute

int init_hardware(p_nes_hardware_info p_hardware_info)
{
	int result = NES_SUCCESS;
	p_nes_rom_info p_rom_info = NULL;
	p_nes_mem_info p_mem_info = NULL;
	p_nes_cpu_info p_cpu_info = NULL;
	p_nes_ppu_info p_ppu_info = NULL;

	p_rom_info = malloc(sizeof(nes_rom_info));
	if (p_rom_info == NULL)
	{
		result = NES_INIT_HARDWARE_ERROR;
		goto end;
	}
	p_mem_info = malloc(sizeof(nes_mem_info));
	if (p_mem_info == NULL)
	{
		result = NES_INIT_HARDWARE_ERROR;
		goto end;
	}
	p_cpu_info = malloc(sizeof(nes_cpu_info));
	if (p_cpu_info == NULL)
	{
		result = NES_INIT_HARDWARE_ERROR;
		goto end;
	}
	p_ppu_info = malloc(sizeof(nes_ppu_info));
	if (p_ppu_info == NULL)
	{
		result = NES_INIT_HARDWARE_ERROR;
		goto end;
	}

	memset(p_rom_info, 0x0, sizeof(nes_rom_info));
	memset(p_mem_info, 0x0, sizeof(nes_mem_info));
	memset(p_cpu_info, 0x0, sizeof(nes_cpu_info));
	memset(p_ppu_info, 0x0, sizeof(nes_ppu_info));
	memset(p_hardware_info, 0x0, sizeof(nes_hardware_info));

	p_hardware_info->rom_info = p_rom_info;
	p_hardware_info->mem_info = p_mem_info;
	p_hardware_info->cpu_info = p_cpu_info;
	p_hardware_info->ppu_info = p_ppu_info;
	p_rom_info->hardware = p_hardware_info;
	p_mem_info->hardware = p_hardware_info;
	p_cpu_info->hardware = p_hardware_info;
	p_ppu_info->hardware = p_hardware_info;

	init_cpu(p_hardware_info->cpu_info);

	init_ppu(p_hardware_info->ppu_info);

	return result;
end:
	if (p_rom_info != NULL)
		free(p_rom_info);
	if (p_mem_info != NULL)
		free(p_mem_info);
	if (p_cpu_info != NULL)
		free(p_cpu_info);
	if (p_ppu_info != NULL)
		free(p_ppu_info);

	return result;
}

static void reset_hardware(p_nes_hardware_info p_hardware_info)
{
	unload_rom(p_hardware_info->rom_info);
	reset_cpu(p_hardware_info->cpu_info);

	memset(p_hardware_info->mem_info, 0x0, sizeof(nes_mem_info));
	p_hardware_info->mem_info->hardware = p_hardware_info;

	reset_ppu(p_hardware_info->ppu_info);
}

// https://wiki.nesdev.org/w/index.php?title=Mapper
inline static int load_mapper(p_nes_hardware_info info)
{
	int result = NES_SUCCESS;

	// load mapper
	switch (info->rom_info->mapper_number)
	{
		NES_CASE_LOAD_MAPPER(000, info);
	default:
		result = NES_MAPPER_NOT_FOUND_ERROR;
	}

	return result;
}

static int load_nes_file(const char* path, p_nes_rom_info info)
{
	int result = load_rom(path, info);
	if (result != NES_SUCCESS)
		return result;

	result = load_mapper(info->hardware);

	return result;
}

int reset_nes(p_nes_hardware_info p_hardware_info, const char* path)
{
	reset_hardware(p_hardware_info);

	int result = load_nes_file(path, p_hardware_info->rom_info);
	if (result != NES_SUCCESS)
		return result;

	// reset vector
	p_hardware_info->cpu_info->registers.PC = read_word(p_hardware_info->mem_info, RESET_VECTOR);
	// 
	// https://wiki.nesdev.org/w/index.php/Emulator_tests
	// testnes.nes Start execution at $C000 and compare execution with a known good log
	//p_hardware_info->cpu_info->registers.PC = 0xC000;

	// cpu_run(p_hardware_info->cpu_info);

	return result;
}

void clear_nes(p_nes_hardware_info p_hardware_info)
{
	if (p_hardware_info->rom_info != NULL)
	{
		unload_rom(p_hardware_info->rom_info);
		free(p_hardware_info->rom_info);
		p_hardware_info->rom_info = NULL;
	}

	if (p_hardware_info->mem_info != NULL)
	{
		free(p_hardware_info->mem_info);
		p_hardware_info->mem_info = NULL;
	}

	if (p_hardware_info->cpu_info != NULL)
	{
		free(p_hardware_info->cpu_info);
		p_hardware_info->cpu_info = NULL;
	}
}