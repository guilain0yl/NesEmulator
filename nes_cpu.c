#include<string.h>
#include<stdlib.h>

#include"nes_cpu.h"
#include"nes_cpu_mem.h"
#include"nes_fast_table.h"

 static void init_fast_table()
{
	byte idx, idx2;

	// init flag n z table
	idx = 0;
	do {
		if (idx == 0)
			g_flag_nz_table[idx] = FLAG_Z;
		else if (idx > 127)
			g_flag_nz_table[idx] = FLAG_N;
		else
			g_flag_nz_table[idx] = 0;

		++idx;
	} while (idx != 0);

	// init asl table
	idx = 0;
	do {
		g_asl_table[idx].value = idx << 1;
		g_asl_table[idx].flag = 0;

		// 128's bits 1000 0000,so if idx greater than 127(0111 1111),when shift left it wiil be overflow.
		if (idx > 127)
			g_asl_table[idx].flag = FLAG_C;

		if (g_asl_table[idx].value == 0)
			g_asl_table[idx].flag |= FLAG_Z;
		else if (g_asl_table[idx].value & 0x80)
			g_asl_table[idx].flag |= FLAG_N;

		++idx;
	} while (idx != 0);

	// init lsr table
	idx = 0;
	do {
		g_lsr_table[idx].value = idx >> 1;
		g_lsr_table[idx].flag = 0;

		//  like 0000 0001,when shift right ,the bit 0 (1) will be underflow
		if (idx & 1)
			g_lsr_table[idx].flag = FLAG_C;

		if (g_lsr_table[idx].value == 0)
			g_lsr_table[idx].flag = FLAG_Z;

		// we don't need to consider negative numbers,because it's shift right.

		++idx;
	} while (idx != 0);

	// init rol table
	for (idx2 = 0; idx2 < 2; ++idx2)
	{
		idx = 0;
		do {
			g_rol_table[idx2][idx].value = (idx << 1) | idx2;
			g_rol_table[idx2][idx].flag = 0;

			// 128's bits 1000 0000,so if idx greater than 127(0111 1111),when shift left it wiil be overflow.
			if (idx > 127)
				g_rol_table[idx2][idx].flag = FLAG_C;

			if (g_rol_table[idx2][idx].value == 0)
				g_rol_table[idx2][idx].flag |= FLAG_Z;
			else if (g_rol_table[idx2][idx].value & 0x80)
				g_rol_table[idx2][idx].flag |= FLAG_N;

			++idx;
		} while (idx != 0);
	}

	// init ror table
	for (idx2 = 0; idx2 < 2; ++idx2)
	{
		idx = 0;
		do {
			// idx2<<7 == idx2*128
			g_ror_table[idx2][idx].value = (idx >> 1) | (idx2 << 7);
			g_ror_table[idx2][idx].flag = 0;

			if (g_ror_table[idx2][idx].value & 1)
				g_ror_table[idx2][idx].flag = FLAG_C;

			else if (g_ror_table[idx2][idx].value == 0)
				g_ror_table[idx2][idx].flag |= FLAG_Z;
			// the value | idx2<<7 , the means the max value is value+128,so we need thought about negative number.
			else if (g_ror_table[idx2][idx].value & 0x80)
				g_ror_table[idx2][idx].flag |= FLAG_N;

			++idx;
		} while (idx != 0);
	}
}

int init_cpu()
{
	init_fast_table();
	cpu_info info;
	memset(&info, 0x0, sizeof(cpu_info));
	// 64KB memory
	info.memory = (byte*)malloc(1024 * 64);
	if (info.memory == NULL)
		return -1;
	info.registers.SP = 0xff;
	init_opcodes(&info);

	return 0;
}

void clean_cpu(p_cpu_info info)
{
	if (info->memory != NULL)
		free(info->memory);
}