#include<stdlib.h>

#include"nes_cpu_opcodes.c"

void opcode_NMI(p_nes_cpu_info info)
{
	push_word(info, info->registers.PC);
	push_byte(info, info->registers.P | FLAG_R);
	set_flag(info, FLAG_I);
	info->registers.PC = read_word(info->memory, NMI_VECTOR);
}

static void init_fast_table()
{
	ubyte idx, idx2;

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
			g_lsr_table[idx].flag |= FLAG_Z;

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

			if (idx & 1)
				g_ror_table[idx2][idx].flag = FLAG_C;

			if (g_ror_table[idx2][idx].value == 0)
				g_ror_table[idx2][idx].flag |= FLAG_Z;
			// the value | idx2<<7 , the means the max value is value+128,so we need thought about negative number.
			else if (g_ror_table[idx2][idx].value & 0x80)
				g_ror_table[idx2][idx].flag |= FLAG_N;

			++idx;
		} while (idx != 0);
	}
}

int init_cpu(p_nes_cpu_info info)
{
	init_fast_table();

	info->memory = malloc(sizeof(nes_mem_info));

	memset(&info->registers, 0x0, sizeof(struct NES_CPU_REGISTERS));
	memset(info->opcodes, 0x0, sizeof(info->opcodes));
	init_opcodes(info);

	return NES_SUCCESS;
}

int reset_cpu(p_nes_cpu_info info)
{
	if (info->memory == NULL)
		return NES_MEMORRY_ERROR;
	memset(info->memory, 0x0, sizeof(nes_mem_info));
	info->memory->p_cpu_info = info;

	info->registers.SP = 0xFD;
	info->registers.P = 0x34 | FLAG_R;
	info->clock = 0;

	return NES_SUCCESS;
}

void do_vblank(p_nes_hardware_info hardware)
{
	SET_B_BLANK(hardware->p_ppu_info);
	if (GEN_NMI(hardware->p_ppu_info))
	{
		opcode_NMI(hardware->p_cpu_info);
	}
}

void cpu_run(p_nes_cpu_info info)
{
#if _DEBUG
	for (size_t i = 0; i < 10000; i++)
	{
#else
	while (info->registers.PC)
	{
#endif
		if (i == 0x6d5)
		{
			int i = 0;
		}

		ubyte opcode = read_byte(info->memory, info->registers.PC);
		info->opcodes[opcode](info);
	}

	do_vblank(info->hardware);
}

void uninit_cpu(p_nes_cpu_info info)
{
	if (info->memory != NULL)
	{
		free(info->memory);
		info->memory = NULL;
	}
}