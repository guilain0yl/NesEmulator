#include"nes_hardware.h"

uword read_w(uword address, p_nes_mem_info info)
{
	if (address >= 0x8000)
	{

	}
	if (address >= 0xC000)
	{
		ubyte low = ((ubyte*)info->current_upper_bank_pointer)[address - 0xC000];
		ubyte high = ((ubyte*)info->current_upper_bank_pointer)[address - 0xC000 + 1];
		uword r = (((uword)high) << 8 | low);
		return r;
	}

	return 0;
}