#include<stdio.h>
#include<string.h>

#include"nes_hardware.h"
#include"nes_mapper.h"

int main()
{
	nes_rom_info rom_info;
	nes_mem_info mem_info;
	nes_cpu_info cpu_info;
	nes_hardware_info hardware;

	memset(&rom_info, 0x0, sizeof(nes_rom_info));
	memset(&mem_info, 0x0, sizeof(nes_mem_info));
	memset(&cpu_info, 0x0, sizeof(nes_cpu_info));
	memset(&hardware, 0x0, sizeof(nes_hardware_info));

	hardware.rom_info = &rom_info;
	hardware.mem_info = &mem_info;
	hardware.cpu_info = &cpu_info;
	rom_info.hardware = &hardware;
	mem_info.hardware = &hardware;
	cpu_info.hardware = &hardware;

	init_cpu(hardware.cpu_info);

	load_rom("F:\\m_project\\NesEmulator\\readme\\nestest.nes", &rom_info);

	nes_mapper_000_reset(&hardware);

	printf("ROM: PRG-ROM: %d x 16kb   CHR-ROM %d x 8kb   Mapper: %03d\n", \
		rom_info.rom_header->prg_size, rom_info.rom_header->chr_size, rom_info.mapper_number);

	uword nmi = read_word(hardware.mem_info, NMI_VECTOR);
	uword reset = read_word(hardware.mem_info, RESET_VECTOR);
	uword irq = read_word(hardware.mem_info, IRQ_VECTOR);

	printf(
		"ROM: NMI: $%04X  RESET: $%04X  IRQ/BRK: $%04X\n",
		(int)nmi, (int)reset, (int)irq
	);

	ubyte s1 = read_byte(hardware.mem_info, nmi);
	ubyte s2 = read_byte(hardware.mem_info, reset);
	ubyte s3 = read_byte(hardware.mem_info, irq);

	//
	printf(
		"NMI:     %s\n"
		"RESET:   %s\n"
		"IRQ/BRK: %s\n",
		opcodes_name[s1], opcodes_name[s2], opcodes_name[s3]
	);

	unload_rom(&rom_info);

	return 0;
}