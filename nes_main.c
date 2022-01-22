#include<stdio.h>
#include<string.h>

#include"nes_hardware.h"

int main()
{
	nes_hardware_info hardware;
	init_hardware(&hardware);

	reset_nes(&hardware, "F:\\m_project\\NesEmulator\\readme\\nestest.nes");

	clear_nes(&hardware);

	return 0;
}