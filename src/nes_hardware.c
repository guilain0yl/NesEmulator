#include<stdlib.h>
#include<string.h>

#include"nes_rom.h"
#include"nes_cpu.h"
#include"nes_mem.h"

#define NES_CASE_LOAD_MAPPER(id,info) case 0x##id: result=nes_mapper_##id##_reset(info);break;

int init_hardware(p_nes_hardware_info p_hardware_info)
{
	int result = NES_SUCCESS;

	p_nes_rom_info p_rom_info = NULL;
	p_nes_cpu_info p_cpu_info = NULL;

	p_rom_info = malloc(sizeof(nes_rom_info));
	if (p_rom_info == NULL)
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

	memset(p_rom_info, 0x0, sizeof(nes_rom_info));
	memset(p_cpu_info, 0x0, sizeof(nes_cpu_info));

	p_hardware_info->p_rom_info = p_rom_info;
	p_hardware_info->p_cpu_info = p_cpu_info;
	p_rom_info->hardware = p_hardware_info;
	p_cpu_info->hardware = p_hardware_info;

	init_cpu(p_hardware_info->p_cpu_info);

end:
	if (p_rom_info != NULL)
		free(p_rom_info);
	if (p_cpu_info != NULL)
	{
		uninit_cpu(p_cpu_info);
		free(p_cpu_info);
	}

	return result;
}

// https://wiki.nesdev.org/w/index.php?title=Mapper
inline static int load_mapper(p_nes_hardware_info info)
{
	int result = NES_SUCCESS;

	// load mapper
	switch (info->p_rom_info->mapper_number)
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

int reset_hardware(p_nes_hardware_info p_hardware_info, const char* path)
{
	int result = NES_SUCCESS;

	unload_rom(p_hardware_info->p_rom_info);
	result = reset_cpu(p_hardware_info->p_cpu_info);
	if (result != NES_SUCCESS)
		return result;

	result = load_nes_file(path, p_hardware_info->p_rom_info);
	if (result != NES_SUCCESS)
		return result;

	p_hardware_info->p_cpu_info->registers.PC =
		read_word(p_hardware_info->p_cpu_info->memory, RESET_VECTOR);

	cpu_run(p_hardware_info->p_cpu_info);

	return result;
}

void uninit_hardware(p_nes_hardware_info p_hardware_info)
{
	if (p_hardware_info->p_rom_info != NULL)
	{
		unload_rom(p_hardware_info->p_rom_info);
		free(p_hardware_info->p_rom_info);
		p_hardware_info->p_rom_info = NULL;
	}

	if (p_hardware_info->p_cpu_info != NULL)
	{
		uninit_cpu(p_hardware_info->p_cpu_info);
		free(p_hardware_info->p_cpu_info);
		p_hardware_info->p_cpu_info = NULL;
	}
}