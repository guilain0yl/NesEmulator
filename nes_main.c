#include<stdio.h>
#include<string.h>

#include"nes_rom.h"


int main()
{
	nes_rom_info info;
	memset(&info, 0x0, sizeof(nes_rom_info));

	load_rom("F:\\m_project\\NesEmulator\\readme\\nestest.nes", &info);

	printf("ROM: PRG-ROM: %d x 16kb   CHR-ROM %d x 8kb   Mapper: %03d\n", \
		info.rom_header->prg_size, info.rom_header->chr_size, info.mapper_number);

	unload_rom(&info);

	return 0;
}