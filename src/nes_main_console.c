#include"nes_hardware.h"


int main()
{
	nes_hardware_info hardware;
	init_hardware(&hardware);

	reset_hardware(&hardware, "F:\\m_project\\NesEmulator\\readme\\nestest.nes");

	uninit_hardware(&hardware);

	return 0;
}