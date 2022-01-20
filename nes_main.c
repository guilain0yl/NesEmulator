#include<stdio.h>
#include<string.h>

#include"nes_hardware.h"
#include"nes_mapper.h"

int main()
{
	nes_rom_info info;
	nes_mem_info mem;
	nes_hardware_info hardware;

	memset(&info, 0x0, sizeof(nes_rom_info));
	memset(&mem, 0x0, sizeof(nes_mem_info));
	memset(&hardware, 0x0, sizeof(nes_hardware_info));

	load_rom("F:\\m_project\\NesEmulator\\readme\\nestest.nes", &info);
	hardware.rom_info = &info;
	hardware.mem_info = &mem;

	nes_mapper_000_reset(&hardware);

	printf("ROM: PRG-ROM: %d x 16kb   CHR-ROM %d x 8kb   Mapper: %03d\n", \
		info.rom_header->prg_size, info.rom_header->chr_size, info.mapper_number);

	uword nmi = read_w(NMI_VECTOR, hardware.mem_info);
	uword reset = read_w(RESET_VECTOR, hardware.mem_info);
	uword irq = read_w(IRQ_VECTOR, hardware.mem_info);

	printf(
		"ROM: NMI: $%04X  RESET: $%04X  IRQ/BRK: $%04X\n",
		(int)nmi, (int)reset, (int)irq
	);

	unload_rom(&info);

	return 0;
}