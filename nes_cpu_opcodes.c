#include"nes_cpu.h"
#include"nes_cpu_mem.h"
#include"nes_fast_table.h"

// http://www.oxyron.de/html/opcodes02.html

// set or reset flag
static inline void set_flag(p_cpu_info info, byte flag)
{
	info->registers.P |= flag;
}
static inline void reset_flag(p_cpu_info info, byte flag)
{
	info->registers.P &= (~flag);
}
static inline set_n_z_flag(p_cpu_info info, byte v)
{
	reset_flag(info, FLAG_N | FLAG_Z);
	set_flag(info, g_flag_nz_table[v]);
}

// return address
static inline byte A_ZP(p_cpu_info info)
{
	byte operand = read_byte(info, info->registers.PC + 1);
	info->registers.PC += 2;
	return operand;
}
static inline byte A_ZPX(p_cpu_info info)
{
	word address = read_byte(info, info->registers.PC + 1) + info->registers.X;
	info->registers.PC += 2;
	return (byte)(address & 0xff);
}
static inline byte A_ZPY(p_cpu_info info)
{
	word address = read_byte(info, info->registers.PC + 1) + info->registers.Y;
	info->registers.PC += 2;
	return (byte)(address & 0xff);
}
static inline word A_ABS(p_cpu_info info)
{
	word operand = read_word(info, info->registers.PC + 1);
	info->registers.PC += 3;
	return operand;
}
static inline word A_ABX(p_cpu_info info)
{
	word address = read_word(info, info->registers.PC + 1) + info->registers.X;
	info->registers.PC += 3;
	return address;
}
static inline word A_ABY(p_cpu_info info)
{
	word address = read_word(info, info->registers.PC + 1) + info->registers.Y;
	info->registers.PC += 3;
	return address;
}
static inline word A_IND(p_cpu_info info)
{
	word operand = read_word(info, info->registers.PC + 1);
	info->registers.PC += 3;
	return operand;
}
static inline void IMP_ACC(p_cpu_info info)
{
	info->registers.PC++;
}
static inline byte IMM(p_cpu_info info)
{
	byte operand = read_byte(info, info->registers.PC + 1);
	info->registers.PC += 2;
	return operand;
}
// Relative addressing is used in branch instructions.
// PC+result
static inline sbyte A_REL(p_cpu_info info)
{
	sbyte operand = (sbyte)read_byte(info, info->registers.PC + 1);
	info->registers.PC += 2;
	return operand;
}
static inline word A_IZX(p_cpu_info info)
{
	word address = read_byte(info, info->registers.PC + 1) + info->registers.X;
	info->registers.PC += 2;
	return read_word(info, address);
}
static inline word A_IZY(p_cpu_info info)
{
	byte operand = read_byte(info, info->registers.PC + 1);
	word address = zp_read_word(info, operand) + info->registers.Y;
	info->registers.PC += 2;
	return address;
}


// return data
static inline byte D_IZX(p_cpu_info info)
{
	return read_byte(info, A_IZX(info));
}
static inline byte D_IZY(p_cpu_info info)
{
	return read_byte(info, A_IZY(info));
}
static inline byte D_ZP(p_cpu_info info)
{
	return zp_read_byte(info, A_ZP(info));
}
static inline byte D_ZPX(p_cpu_info info)
{
	return zp_read_byte(info, A_ZPX(info));
}
static inline byte D_ZPY(p_cpu_info info)
{
	return zp_read_byte(info, A_ZPY(info));
}
static inline byte D_ABS(p_cpu_info info)
{
	return read_byte(info, A_ABS(info));
}
static inline word D_ABS_W(p_cpu_info info)
{
	return read_word(info, A_ABS(info));
}
static inline byte D_ABX(p_cpu_info info)
{
	return read_byte(info, A_ABX(info));
}
static inline byte D_ABY(p_cpu_info info)
{
	return read_byte(info, A_ABY(info));
}

// opcode implement
typedef byte(*func_byte)(p_cpu_info info);
typedef sbyte(*func_sbyte)(p_cpu_info info);
typedef word(*func_word)(p_cpu_info info);
// 
static inline void ora_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	info->registers.A |= data;
	set_n_z_flag(info, info->registers.A);
}
static inline void and_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	info->registers.A &= data;
	set_n_z_flag(info, info->registers.A);
}
static inline void eor_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	info->registers.A ^= data;
	set_n_z_flag(info, info->registers.A);
}
// formula url: http://www.6502.org/tutorials/65c816opcodes.html#6.1.1.1
// v flag url: http://www.6502.org/tutorials/vflag.html#2.2
// The formula for ADC  accumulator = accumulator + data + carry
static inline void adc_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	word tmp = info->registers.A + data + (info->registers.P & FLAG_C);
	byte tmp1 = (byte)tmp;
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z | FLAG_C);
	// V indicates whether the result of an addition or subraction is outside the range -128 to 127
	set_flag(info, g_flag_nz_table[tmp1] | \
		// ~(info->registers.A ^ data) , ensure A and data have same sign
		// (info->registers.A ^ tmp1) , ensure A and result have same sign,if not, overflow,like -1 + -1 = 2
		((~(info->registers.A ^ data) & (info->registers.A ^ tmp1) & 0x80) ? FLAG_V : 0) | \
		(tmp > 0xFF));
	info->registers.A = tmp1;
}
// The formula for SBC  accumulator = accumulator - data - 1 + carry
static inline void sbc_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	word tmp = info->registers.A - data - (~info->registers.P & FLAG_C);
	byte tmp1 = (byte)tmp;
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z | FLAG_C);
	// see v flag url
	// After SBC or CMP, this flag will be set if no borrow was the result, or alternatively a "greater than or equal" result.
	// When the subtraction result is 0 to 255, the carry is set.
	// When the subtraction result is less than 0, the carry is cleared.
	// for example. 0x1 - 0x2 == 1 0000 0001 - 0000 0010 = 1111 1111 1111 = 0xFF = -1
	set_flag(info, g_flag_nz_table[tmp1] | \
		// (info->registers.A ^ data) , ensure A and data haven't same sign
		// (info->registers.A ^ tmp1) , ensure A and result have same sign,if not, overflow,like 2 - -2 = -2
		(((info->registers.A ^ data) & (info->registers.A ^ tmp1) & 0x80) ? FLAG_V : 0) | \
		(tmp < 0x100));
	info->registers.A = tmp1;
}
static inline void cmp_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	word tmp = info->registers.A - data;
	byte tmp1 = (byte)tmp;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	set_flag(info, g_flag_nz_table[tmp1] | (tmp < 0x100));
}
static inline void cpx_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	word tmp = info->registers.X - data;
	byte tmp1 = (byte)tmp;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	set_flag(info, g_flag_nz_table[tmp1] | (tmp < 0x100));
}
static inline void cpy_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	word tmp = info->registers.Y - data;
	byte tmp1 = (byte)tmp;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	set_flag(info, g_flag_nz_table[tmp1] | (tmp < 0x100));
}
static inline void dec_iml_a(p_cpu_info info, func_word func)
{
	word address = func(info);
	byte data = read_byte(info, address) - 1;
	write_byte(info, address, data);
	set_flag(info, g_flag_nz_table[data]);
}
static inline void dec_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte address = func(info);
	byte data = zp_read_byte(info, address) - 1;
	zp_write_byte(info, address, data);
	set_flag(info, g_flag_nz_table[data]);
}
static inline void inc_iml_a(p_cpu_info info, func_word func)
{
	word address = func(info);
	byte data = read_byte(info, address) + 1;
	write_byte(info, address, data);
	set_flag(info, g_flag_nz_table[data]);
}
static inline void inc_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte address = func(info);
	byte data = zp_read_byte(info, address) + 1;
	zp_write_byte(info, address, data);
	set_flag(info, g_flag_nz_table[data]);
}
static inline void asl_iml_a(p_cpu_info info, func_word func)
{
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	word address = func(info);
	byte data = read_byte(info, address);
	write_byte(info, address, g_asl_table[data].value);
	set_flag(info, g_asl_table[data].flag);
}
static inline void asl_iml_a_zp(p_cpu_info info, func_byte func)
{
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte address = func(info);
	byte data = zp_read_byte(info, address);
	zp_write_byte(info, address, g_asl_table[data].value);
	set_flag(info, g_asl_table[data].flag);
}
static inline void rol_iml_a(p_cpu_info info, func_word func)
{
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	word address = func(info);
	byte data = read_byte(info, address);
	write_byte(info, address, g_rol_table[tmp][data].value);
	set_flag(info, g_rol_table[tmp][data].flag);
}
static inline void rol_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte address = func(info);
	byte data = zp_read_byte(info, address);
	zp_write_byte(info, address, g_rol_table[tmp][data].value);
	set_flag(info, g_rol_table[tmp][data].flag);
}
static inline void lsr_iml_a(p_cpu_info info, func_word func)
{
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	word address = func(info);
	byte data = read_byte(info, address);
	write_byte(info, address, g_lsr_table[data].value);
	set_flag(info, g_lsr_table[data].flag);
}
static inline void lsr_iml_a_zp(p_cpu_info info, func_byte func)
{
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte address = func(info);
	byte data = zp_read_byte(info, address);
	zp_write_byte(info, address, g_lsr_table[data].value);
	set_flag(info, g_lsr_table[data].flag);
}
static inline void ror_iml_a(p_cpu_info info, func_word func)
{
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	word address = func(info);
	byte data = read_byte(info, address);
	write_byte(info, address, g_ror_table[tmp][data].value);
	set_flag(info, g_ror_table[tmp][data].flag);
}
static inline void ror_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte address = func(info);
	byte data = zp_read_byte(info, address);
	zp_write_byte(info, address, g_ror_table[tmp][data].value);
	set_flag(info, g_ror_table[tmp][data].flag);
}
// move commands
static inline void lda_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	info->registers.A = data;
	set_n_z_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void sta_iml_a(p_cpu_info info, func_word func)
{
	word address = func(info);
	write_byte(info, address, info->registers.A);
}
static inline void sta_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte address = func(info);
	zp_write_byte(info, address, info->registers.A);
}
static inline void ldx_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	info->registers.X = data;
	set_n_z_flag(info, g_flag_nz_table[info->registers.X]);
}
static inline void stx_iml_a(p_cpu_info info, func_word func)
{
	word address = func(info);
	write_byte(info, address, info->registers.X);
}
static inline void stx_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte address = func(info);
	zp_write_byte(info, address, info->registers.X);
}
static inline void ldy_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	info->registers.Y = data;
	set_n_z_flag(info, g_flag_nz_table[info->registers.Y]);
}
static inline void sty_iml_a(p_cpu_info info, func_word func)
{
	word address = func(info);
	write_byte(info, address, info->registers.Y);
}
static inline void sty_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte address = func(info);
	zp_write_byte(info, address, info->registers.Y);
}
// branch
static inline void branch_true(p_cpu_info info, byte flag)
{
	sbyte operand = A_REL(info) + (sbyte)info->registers.PC;
	if (info->registers.P & flag)
		info->registers.PC = operand;
}
static inline void branch_false(p_cpu_info info, byte flag)
{
	sbyte operand = A_REL(info) + (sbyte)info->registers.PC;
	if (!(info->registers.P & flag))
		info->registers.PC = operand;
}
// Illegal opcodes
static inline void slo_iml_a(p_cpu_info info, func_word func)
{
	// {adr}:={adr}*2 like ASL
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	word address = func(info);
	byte data = read_byte(info, address);
	write_byte(info, address, g_asl_table[data].value);
	set_flag(info, g_asl_table[data].flag);

	// A:=A or {adr}
	info->registers.A |= g_asl_table[data].value;
	// override pre flag
	set_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void slo_iml_a_zp(p_cpu_info info, func_byte func)
{
	// {adr}:={adr}*2 like ASL
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte address = func(info);
	byte data = zp_read_byte(info, address);
	zp_write_byte(info, address, g_asl_table[data].value);
	set_flag(info, g_asl_table[data].flag);

	// A:=A or {adr}
	info->registers.A |= g_asl_table[data].value;
	// override pre flag
	set_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void rla_iml_a(p_cpu_info info, func_word func)
{
	// {adr}:={adr}rol
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	word address = func(info);
	byte data = read_byte(info, address);
	write_byte(info, address, g_rol_table[tmp][data].value);
	set_flag(info, g_rol_table[tmp][data].flag);

	// A:=A and {adr}
	info->registers.A &= g_rol_table[tmp][data].value;
	// override pre flag
	set_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void rla_iml_a_zp(p_cpu_info info, func_byte func)
{
	// {adr}:={adr}rol
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte address = func(info);
	byte data = zp_read_byte(info, address);
	zp_write_byte(info, address, g_rol_table[tmp][data].value);
	set_flag(info, g_rol_table[tmp][data].flag);

	// A:=A and {adr}
	info->registers.A |= g_rol_table[tmp][data].value;
	// override pre flag
	set_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void sre_iml_a(p_cpu_info info, func_word func)
{
	// {adr}:={adr}/2 LSR
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	word address = func(info);
	byte data = read_byte(info, address);
	write_byte(info, address, g_lsr_table[data].value);
	set_flag(info, g_lsr_table[data].flag);

	// A:=A exor {adr}
	info->registers.A &= g_lsr_table[data].value;
	// override pre flag
	set_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void sre_iml_a_zp(p_cpu_info info, func_byte func)
{
	// {adr}:={adr}/2 LSR
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte address = func(info);
	byte data = zp_read_byte(info, address);
	zp_write_byte(info, address, g_lsr_table[data].value);
	set_flag(info, g_lsr_table[data].flag);

	// A:=A exor {adr}
	info->registers.A ^= g_lsr_table[data].value;
	// override pre flag
	set_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void rra_iml_a(p_cpu_info info, func_word func)
{
	// {adr}:={adr}ror
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	word address = func(info);
	byte data = read_byte(info, address);
	write_byte(info, address, g_ror_table[tmp][data].value);
	set_flag(info, g_ror_table[tmp][data].flag);

	// A:=A adc {adr}
	data = g_ror_table[tmp][data].value;
	word res = info->registers.A + data + (info->registers.P & FLAG_C);
	byte tmp1 = (byte)res;
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z | FLAG_C);
	// V indicates whether the result of an addition or subraction is outside the range -128 to 127
	set_flag(info, g_flag_nz_table[tmp1] | \
		// ~(info->registers.A ^ data) , ensure A and data have same sign
		// (info->registers.A ^ tmp1) , ensure A and result have same sign,if not, overflow,like -1 + -1 = 2
		((~(info->registers.A ^ data) & (info->registers.A ^ tmp1) & 0x80) ? FLAG_V : 0) | \
		(res > 0xFF));
	info->registers.A = tmp1;
}
static inline void rra_iml_a_zp(p_cpu_info info, func_byte func)
{
	// first instruction. {adr}:={adr}ror
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte address = func(info);
	byte data = zp_read_byte(info, address);
	zp_write_byte(info, address, g_ror_table[tmp][data].value);
	set_flag(info, g_ror_table[tmp][data].flag);

	// second instruction.A:=A adc {adr}
	data = g_ror_table[tmp][data].value;
	word res = info->registers.A + data + (info->registers.P & FLAG_C);
	byte tmp1 = (byte)res;
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z | FLAG_C);
	// V indicates whether the result of an addition or subraction is outside the range -128 to 127
	set_flag(info, g_flag_nz_table[tmp1] | \
		// ~(info->registers.A ^ data) , ensure A and data have same sign
		// (info->registers.A ^ tmp1) , ensure A and result have same sign,if not, overflow,like -1 + -1 = 2
		((~(info->registers.A ^ data) & (info->registers.A ^ tmp1) & 0x80) ? FLAG_V : 0) | \
		(res > 0xFF));
	info->registers.A = tmp1;

}
static inline void sax_iml_a(p_cpu_info info, func_word func)
{
	word address = func(info);
	write_byte(info, address, info->registers.A & info->registers.X);
}
static inline void sax_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte address = func(info);
	zp_write_byte(info, address, info->registers.A & info->registers.X);
}
static inline void lax_iml_d(p_cpu_info info, func_byte func)
{
	byte data = func(info);
	info->registers.A = info->registers.X = data;
	set_n_z_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void dcp_iml_a(p_cpu_info info, func_word func)
{
	word address = func(info);
	byte data = read_byte(info, address) - 1;
	write_byte(info, address, data);
	word tmp = (word)info->registers.A - data;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	set_flag(info, g_flag_nz_table[tmp & 0xFF] | ((tmp < 0x100) ? FLAG_C : 0));
}
static inline void dcp_iml_a_zp(p_cpu_info info, func_byte func)
{
	byte address = func(info);
	byte data = zp_read_byte(info, address) - 1;
	zp_write_byte(info, address, data);
	word tmp = (word)info->registers.A - data;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	set_flag(info, g_flag_nz_table[tmp & 0xFF] | ((tmp < 0x100) ? FLAG_C : 0));
}
static inline void isc_iml_a(p_cpu_info info, func_word func)
{
	// {adr}:={adr}+1
	word address = func(info);
	byte data = read_byte(info, address) + 1;
	write_byte(info, address, data);

	// A:=A-{adr} like sbc {addr}
	word tmp = info->registers.A - data - (~info->registers.P & FLAG_C);
	byte tmp1 = (byte)tmp;
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z | FLAG_C);
	set_flag(info, g_flag_nz_table[tmp1] | \
		(((info->registers.A ^ data) & (info->registers.A ^ tmp1) & 0x80) ? FLAG_V : 0) | \
		(tmp < 0x100));
	info->registers.A = tmp1;
}
static inline void isc_iml_a_zp(p_cpu_info info, func_byte func)
{
	// {adr}:={adr}+1
	byte address = func(info);
	byte data = zp_read_byte(info, address) + 1;
	zp_write_byte(info, address, data);

	// A:=A-{adr} like sbc {addr}
	word tmp = info->registers.A - data - (~info->registers.P & FLAG_C);
	byte tmp1 = (byte)tmp;
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z | FLAG_C);
	set_flag(info, g_flag_nz_table[tmp1] | \
		(((info->registers.A ^ data) & (info->registers.A ^ tmp1) & 0x80) ? FLAG_V : 0) | \
		(tmp < 0x100));
	info->registers.A = tmp1;
}

// opcodes
// 0x00-0x0F
static inline void BRK(p_cpu_info info)
{
	// 00
	IMP_ACC(info);
	push_word(info, info->registers.PC);
	set_flag(info, FLAG_B | FLAG_R);
	push_word(info, info->registers.P);
	set_flag(info, FLAG_I);
	reset_flag(info, FLAG_D);
	info->registers.PC = read_word(info, IRQ_BRK_VECTOR);
}
static inline void ORA_IZX(p_cpu_info info)
{
	// 01
	ora_iml_d(info, D_IZX);
}
static inline void SLO_IZX(p_cpu_info info)
{
	// 03
	slo_iml_a(info, A_IZX);
}
static inline void NOP_ZP(p_cpu_info info)
{
	// 04
	A_ZP(info);
}
static inline void ORA_ZP(p_cpu_info info)
{
	// 05
	ora_iml_d(info, D_ZP);
}
static inline void ASL_ZP(p_cpu_info info)
{
	// 06
	asl_iml_a_zp(info, A_ZP);
}
static inline void SLO_ZP(p_cpu_info info)
{
	// 07
	slo_iml_a_zp(info, A_ZP);
}
static inline void PHP(p_cpu_info info)
{
	// 08
	IMP_ACC(info);
	set_flag(info, FLAG_B | FLAG_R);
	push_word(info, info->registers.P);
}
static inline void ORA_IMM(p_cpu_info info)
{
	// 09
	ora_iml_d(info, IMM);
}
static inline void ASL(p_cpu_info info)
{
	// 0A
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	IMP_ACC(info);
	info->registers.A = g_asl_table[info->registers.A].value;
	set_flag(info, g_asl_table[info->registers.A].flag);
}
static inline void ANC_IMM(p_cpu_info info)
{
	// 0B 2B
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	byte data = IMM(info);
	info->registers.A &= data;
	set_n_z_flag(info, g_flag_nz_table[info->registers.A]);
	// this command performs an AND operation only, but bit 7 is put into the carry, as if the ASL/ROL would have been executed.
	if (info->registers.A & 0x80)
		set_flag(info, FLAG_C);
}
static inline void NOP_ABS(p_cpu_info info)
{
	// 0C
	A_ABS(info);
}
static inline void ORA_ABS(p_cpu_info info)
{
	// 0D
	ora_iml_d(info, D_ABS);
}
static inline void ASL_ABS(p_cpu_info info)
{
	// 0E
	asl_iml_a(info, A_ABS);
}
static inline void SLO_ABS(p_cpu_info info)
{
	// 0F
	slo_iml_a(info, A_ABS);
}
// 0x10-0x1f
static inline void BLP_REL(p_cpu_info info)
{
	// 10
	branch_false(info, FLAG_N);
}
static inline void ORA_IZY(p_cpu_info info)
{
	// 11
	ora_iml_d(info, D_IZY);
}
static inline void SLO_IZY(p_cpu_info info)
{
	// 13
	slo_iml_a(info, A_IZY);
}
static inline void NOP_ZPX(p_cpu_info info)
{
	// 14
	A_ZPX(info);
}
static inline void ORA_ZPX(p_cpu_info info)
{
	// 15
	ora_iml_d(info, D_ZPX);
}
static inline void ASL_ZPX(p_cpu_info info)
{
	// 16
	asl_iml_a_zp(info, A_ZPX);
}
static inline void SLO_ZPX(p_cpu_info info)
{
	// 17
	slo_iml_a_zp(info, A_ZPX);
}
static inline void CLC(p_cpu_info info)
{
	// 18
	IMP_ACC(info);
	reset_flag(info, FLAG_C);
}
static inline void ORA_ABY(p_cpu_info info)
{
	// 19
	ora_iml_d(info, D_ABY);
}
static inline void NOP(p_cpu_info info)
{
	// 1A
	IMP_ACC(info);
}
static inline void SLO_ABY(p_cpu_info info)
{
	// 1B
	slo_iml_a(info, A_ABY);
}
static inline void NOP_ABX(p_cpu_info info)
{
	// 1C
	A_ABX(info);
}
static inline void ORA_ABX(p_cpu_info info)
{
	// 1D
	ora_iml_d(info, D_ABX);
}
static inline void ASL_ABX(p_cpu_info info)
{
	// 1E
	asl_iml_a(info, A_ABX);
}
static inline void SLO_ABX(p_cpu_info info)
{
	// 1F
	slo_iml_a(info, A_ABX);
}
// 0x20-0x2F
static inline void JSR_ABS(p_cpu_info info)
{
	// 20
	word data = D_ABS_W(info);
	push_word(info, info->registers.PC);
	info->registers.PC = data;
}
static inline void AND_IZX(p_cpu_info info)
{
	// 21
	and_iml_d(info, D_IZX);
}
static inline void RLA_IZX(p_cpu_info info)
{
	// 23
	rla_iml_a(info, A_IZX);
}
static inline void BIT_ZP(p_cpu_info info)
{
	// 24
	byte data = D_ZP(info);
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z);
	set_flag(info, (data & (FLAG_N | FLAG_V)) | ((data & info->registers.A) ? 0 : FLAG_Z));
}
static inline void AND_ZP(p_cpu_info info)
{
	// 25
	and_iml_d(info, D_ZP);
}
static inline void ROL_ZP(p_cpu_info info)
{
	// 26
	rol_iml_a_zp(info, A_ZP);
}
static inline void RLA_ZP(p_cpu_info info)
{
	// 27
	rla_iml_a_zp(info, A_ZP);
}
static inline void PLP(p_cpu_info info)
{
	// 28
	IMP_ACC(info);
	info->registers.P = pop_byte(info);
}
static inline void AND_IMM(p_cpu_info info)
{
	// 29
	and_iml_d(info, IMM);
}
static inline void ROL(p_cpu_info info)
{
	// 2A
	IMP_ACC(info);
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	set_flag(info, g_rol_table[tmp][info->registers.A].flag);
	info->registers.A = g_rol_table[tmp][info->registers.A].value;
}
static inline void BIT_ABS(p_cpu_info info)
{
	// 2C
	byte data = D_ABS(info);
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z);
	set_flag(info, (data & (FLAG_N | FLAG_V)) | ((data & info->registers.A) ? 0 : FLAG_Z));
}
static inline void AND_ABS(p_cpu_info info)
{
	// 2D
	and_iml_d(info, D_ABS);
}
static inline void ROL_ABS(p_cpu_info info)
{
	// 2E
	rol_iml_a(info, A_ABS);
}
static inline void RLA_ABS(p_cpu_info info)
{
	// 2F
	rla_iml_a(info, A_ABS);
}
// 0x30-0x3F
static inline void BMI_REL(p_cpu_info info)
{
	// 30
	branch_true(info, FLAG_N);
}
static inline void AND_IZY(p_cpu_info info)
{
	// 31
	and_iml_d(info, D_IZY);
}
static inline void RLA_IZY(p_cpu_info info)
{
	// 33
	rla_iml_a(info, A_IZY);
}
static inline void AND_ZPX(p_cpu_info info)
{
	// 35
	and_iml_d(info, D_ZPX);
}
static inline void ROL_ZPX(p_cpu_info info)
{
	// 36
	rol_iml_a_zp(info, A_ZPX);
}
static inline void RLA_ZPX(p_cpu_info info)
{
	// 37
	rla_iml_a_zp(info, A_ZPX);
}
static inline void SEC(p_cpu_info info)
{
	// 38
	IMP_ACC(info);
	set_flag(info, FLAG_C);
}
static inline void AND_ABY(p_cpu_info info)
{
	// 39
	and_iml_d(info, D_ABY);
}
static inline void RLA_ABY(p_cpu_info info)
{
	// 3B
	rla_iml_a(info, A_ABY);
}
static inline void AND_ABX(p_cpu_info info)
{
	// 3D
	and_iml_d(info, D_ABX);
}
static inline void ROL_ABX(p_cpu_info info)
{
	// 3E
	rol_iml_a(info, A_ABX);
}
static inline void RLA_ABX(p_cpu_info info)
{
	// 3F
	rla_iml_a(info, A_ABX);
}
// 0x40-0x4F
static inline void RTI(p_cpu_info info)
{
	// 40
	IMP_ACC(info);
	info->registers.P = pop_byte(info);
	set_flag(info, FLAG_R);
	info->registers.PC = pop_word(info);
}
static inline void EOR_IZX(p_cpu_info info)
{
	// 41
	eor_iml_d(info, D_IZX);
}
static inline void SRE_IZX(p_cpu_info info)
{
	// 43
	sre_iml_a(info, A_IZX);
}
static inline void EOR_ZP(p_cpu_info info)
{
	// 45
	eor_iml_d(info, D_ZP);
}
static inline void LSR_ZP(p_cpu_info info)
{
	// 46
	lsr_iml_a_zp(info, A_ZP);
}
static inline void SRE_ZP(p_cpu_info info)
{
	// 47
	sre_iml_a_zp(info, A_ZP);
}
static inline void PHA(p_cpu_info info)
{
	// 48
	IMP_ACC(info);
	push_byte(info, info->registers.A);
}
static inline void EOR_IMM(p_cpu_info info)
{
	// 49
	eor_iml_d(info, IMM);
}
static inline void LSR(p_cpu_info info)
{
	// 4A
	IMP_ACC(info);
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	set_flag(info, g_lsr_table[info->registers.A].flag);
	info->registers.A = g_lsr_table[info->registers.A].value;
}
static inline void ALR_IMM(p_cpu_info info)
{
	// 4B
	byte data = IMM(info) & info->registers.A;
	reset_flag(info, FLAG_N | FLAG_C | FLAG_Z);
	info->registers.A = g_lsr_table[data].value;
	set_flag(info, g_lsr_table[data].flag);
}
static inline void JMP_ABS(p_cpu_info info)
{
	// 4C
	info->registers.PC = D_ABS_W(info);
}
static inline void EOR_ABS(p_cpu_info info)
{
	// 4D
	eor_iml_d(info, D_ABS);
}
static inline void LSR_ABS(p_cpu_info info)
{
	// 4E
	lsr_iml_a(info, A_ABS);
}
static inline void SRE_ABS(p_cpu_info info)
{
	// 4F
	sre_iml_a(info, A_ABS);
}
// 0x50-0x5F
static inline void BVC_REL(p_cpu_info info)
{
	// 50
	branch_false(info, FLAG_V);
}
static inline void EOR_IZY(p_cpu_info info)
{
	// 51
	eor_iml_d(info, D_IZY);
}
static inline void SRE_IZY(p_cpu_info info)
{
	// 53
	sre_iml_a(info, A_IZY);
}
static inline void EOR_ZPX(p_cpu_info info)
{
	// 55
	eor_iml_d(info, D_ZPX);
}
static inline void LSR_ZPX(p_cpu_info info)
{
	// 56
	lsr_iml_a_zp(info, A_ZPX);
}
static inline void SRE_ZPX(p_cpu_info info)
{
	// 57
	sre_iml_a_zp(info, A_ZPX);
}
static inline void CLI(p_cpu_info info)
{
	// 58
	IMP_ACC(info);
	reset_flag(info, FLAG_I);
}
static inline void EOR_ABY(p_cpu_info info)
{
	// 59
	eor_iml_d(info, D_ABY);
}
static inline void SRE_ABY(p_cpu_info info)
{
	// 5B
	sre_iml_a(info, A_ABY);
}
static inline void EOR_ABX(p_cpu_info info)
{
	// 5D
	eor_iml_d(info, D_ABX);
}
static inline void LSR_ABX(p_cpu_info info)
{
	// 5E
	lsr_iml_a(info, A_ABX);
}
static inline void SRE_ABX(p_cpu_info info)
{
	// 5F
	sre_iml_a(info, A_ABX);
}
// 0x60-0x6F
static inline void RTS(p_cpu_info info)
{
	// 60
	IMP_ACC(info);
	info->registers.PC = pop_word(info);
}
static inline void ADC_IZX(p_cpu_info info)
{
	// 61
	adc_iml_d(info, D_IZX);
}
static inline void RRA_IZX(p_cpu_info info)
{
	// 63
	rra_iml_a(info, A_IZX);
}
static inline void ADC_ZP(p_cpu_info info)
{
	// 65
	adc_iml_d(info, D_ZP);
}
static inline void ROR_ZP(p_cpu_info info)
{
	// 66
	ror_iml_a_zp(info, A_ZP);
}
static inline void RRA_ZP(p_cpu_info info)
{
	// 67
	rra_iml_a_zp(info, A_ZP);
}
static inline void PLA(p_cpu_info info)
{
	// 68
	IMP_ACC(info);
	info->registers.A = pop_byte(info);
	set_n_z_flag(info, info->registers.A);
}
static inline void ADC_IMM(p_cpu_info info)
{
	// 69
	adc_iml_d(info, IMM);
}
static inline void ROR(p_cpu_info info)
{
	// 6A
	IMP_ACC(info);
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_Z | FLAG_C);
	set_flag(info, g_ror_table[tmp][info->registers.A].flag);
	info->registers.A = g_ror_table[tmp][info->registers.A].value;
}
static inline void ARR_IMM(p_cpu_info info)
{
	// 6B
	// Similar to AND #i then ROR A, except sets the flags differently. 
	// N and Z are normal, but C is bit 6 and V is bit 6 xor bit 5. 
	byte tmp = info->registers.P & FLAG_C;
	reset_flag(info, FLAG_N | FLAG_V | FLAG_Z | FLAG_C);
	byte data = info->registers.A & IMM(info);
	info->registers.A = g_rol_table[tmp][data].value;
	set_n_z_flag(info, info->registers.A);
	set_flag(info, ((info->registers.A & 0x40) ? FLAG_C : 0));
	set_flag(info, (info->registers.A << 1 ^ info->registers.A) & FLAG_V);
}
static inline void JMP_IND(p_cpu_info info)
{
	// 6C
	word address = A_IND(info);
	info->registers.PC = read_word(info, address);
}
static inline void ADC_ABS(p_cpu_info info)
{
	// 6D
	adc_iml_d(info, D_ABS);
}
static inline void ROR_ABS(p_cpu_info info)
{
	// 6E
	ror_iml_a(info, A_ABS);
}
static inline void RRA_ABS(p_cpu_info info)
{
	// 6F
	rra_iml_a(info, A_ABS);
}
// 0x70-0x7F
static inline void BVS_REL(p_cpu_info info)
{
	// 70
	branch_true(info, FLAG_V);
}
static inline void ADC_IZY(p_cpu_info info)
{
	// 71
	adc_iml_d(info, D_IZY);
}
static inline void RRA_IZY(p_cpu_info info)
{
	// 73
	rra_iml_a(info, A_IZY);
}
static inline void ADC_ZPX(p_cpu_info info)
{
	// 75
	adc_iml_d(info, D_ZPX);
}
static inline void ROR_ZPX(p_cpu_info info)
{
	// 76
	ror_iml_a_zp(info, A_ZPX);
}
static inline void RRA_ZPX(p_cpu_info info)
{
	// 77
	rra_iml_a_zp(info, A_ZPX);
}
static inline void SEI(p_cpu_info info)
{
	// 78
	IMP_ACC(info);
	set_flag(info, FLAG_I);
}
static inline void ADC_ABY(p_cpu_info info)
{
	// 79
	adc_iml_d(info, D_ABY);
}
static inline void RRA_ABY(p_cpu_info info)
{
	// 7B
	rra_iml_a(info, A_ABY);
}
static inline void ADC_ABX(p_cpu_info info)
{
	// 7D
	adc_iml_d(info, D_ABX);
}
static inline void ROR_ABX(p_cpu_info info)
{
	// 7E
	ror_iml_a(info, A_ABX);
}
static inline void RRA_ABX(p_cpu_info info)
{
	// 7F
	rra_iml_a(info, A_ABX);
}
// 0x80-0x8F
static inline void NOP_IMM(p_cpu_info info)
{
	// 80
	IMM(info);
}
static inline void STA_IZX(p_cpu_info info)
{
	// 81
	sta_iml_a(info, A_IZX);
}
static inline void SAX_IZX(p_cpu_info info)
{
	// 83
	sax_iml_a(info, A_IZX);
}
static inline void STY_ZP(p_cpu_info info)
{
	// 84
	sty_iml_a_zp(info, A_ZP);
}
static inline void STA_ZP(p_cpu_info info)
{
	// 85
	sta_iml_a_zp(info, A_ZP);
}
static inline void STX_ZP(p_cpu_info info)
{
	// 86
	stx_iml_a_zp(info, A_ZP);
}
static inline void SAX_ZP(p_cpu_info info)
{
	// 87
	sax_iml_a_zp(info, A_ZP);
}
static inline void DEY(p_cpu_info info)
{
	// 88
	IMP_ACC(info);
	info->registers.Y -= 1;
	set_n_z_flag(info, g_flag_nz_table[info->registers.Y]);
}
static inline void TXA(p_cpu_info info)
{
	// 8A
	// A:=X
	IMP_ACC(info);
	info->registers.A = info->registers.X;
	set_n_z_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void XAA_IMM(p_cpu_info info)
{
	// 8B
	// A:=X&#{imm}
	byte data = IMM(info);
	info->registers.A = info->registers.X & data;
	set_n_z_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void STY_ABS(p_cpu_info info)
{
	// 8C
	sty_iml_a(info, A_ABS);
}
static inline void STA_ABS(p_cpu_info info)
{
	// 8D
	sta_iml_a(info, A_ABS);
}
static inline void STX_ABS(p_cpu_info info)
{
	// 8E
	stx_iml_a(info, A_ABS);
}
static inline void SAX_ABS(p_cpu_info info)
{
	// 8F
	sax_iml_a(info, A_ABS);
}
// 0x90-0x9F
static inline void BCC_REL(p_cpu_info info)
{
	// 90
	branch_false(info, FLAG_C);
}
static inline void STA_IZY(p_cpu_info info)
{
	// 91
	sta_iml_a(info, A_IZY);
}
static inline void AHX_IZY(p_cpu_info info)
{
	// 93
	// {adr}:=A&X&H
	// nop
}
static inline void STY_ZPX(p_cpu_info info)
{
	// 94
	sty_iml_a_zp(info, A_ZPX);
}
static inline void STA_ZPX(p_cpu_info info)
{
	// 95
	sta_iml_a_zp(info, A_ZPX);
}
static inline void STX_ZPY(p_cpu_info info)
{
	// 96
	stx_iml_a_zp(info, A_ZPY);
}
static inline void SAX_ZPY(p_cpu_info info)
{
	// 97
	sax_iml_a_zp(info, A_ZPY);
}
static inline void TYA(p_cpu_info info)
{
	// 98
	IMP_ACC(info);
	info->registers.A = info->registers.Y;
	set_n_z_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void STA_ABY(p_cpu_info info)
{
	// 99
	sta_iml_a(info, A_ABY);
}
static inline void TXS(p_cpu_info info)
{
	// 9A
	IMP_ACC(info);
	info->registers.SP = info->registers.X;
}
static inline void TAS_ABY(p_cpu_info info)
{
	// 9B
	// S:=A&X {adr}:=S&H
	// nop
}
static inline void SHY_ABX(p_cpu_info info)
{
	// 9C
	// {adr}:=Y&H
	// nop
}
static inline void STA_ABX(p_cpu_info info)
{
	// 9D
	sta_iml_a(info, A_ABX);
}
static inline void SHX_ABY(p_cpu_info info)
{
	// 9E
	// {adr}:=X&H
	// nop
}
static inline void AHX_ABY(p_cpu_info info)
{
	// 9F
	// {adr}:=A&X&H
	// nop
}
// 0xA0-0xAF
static inline void LDY_IMM(p_cpu_info info)
{
	// A0
	ldy_iml_d(info, IMM);
}
static inline void LDA_IZX(p_cpu_info info)
{
	// A1
	lda_iml_d(info, D_IZX);
}
static inline void LDX_IMM(p_cpu_info info)
{
	// A2
	ldx_iml_d(info, IMM);
}
static inline void LAX_IZX(p_cpu_info info)
{
	// A3
	ldx_iml_d(info, D_IZX);
}
static inline void LDY_ZP(p_cpu_info info)
{
	// A4
	ldy_iml_d(info, D_ZP);
}
static inline void LDA_ZP(p_cpu_info info)
{
	// A5
	lda_iml_d(info, D_ZP);
}
static inline void LDX_ZP(p_cpu_info info)
{
	// A6
	ldx_iml_d(info, D_ZP);
}
static inline void LAX_ZP(p_cpu_info info)
{
	// A7
	lax_iml_d(info, D_ZP);
}
static inline void TAY(p_cpu_info info)
{
	// A8
	IMP_ACC(info);
	info->registers.Y = info->registers.A;
	set_n_z_flag(info, g_flag_nz_table[info->registers.Y]);
}
static inline void LDA_IMM(p_cpu_info info)
{
	// A9
	lda_iml_d(info, IMM);
}
static inline void TAX(p_cpu_info info)
{
	// AA
	IMP_ACC(info);
	info->registers.X = info->registers.A;
	set_n_z_flag(info, g_flag_nz_table[info->registers.X]);
}
static inline void LAX_IMM(p_cpu_info info)
{
	// AB
	lax_iml_d(info, IMM);
}
static inline void LDY_ABS(p_cpu_info info)
{
	// AC
	ldy_iml_d(info, D_ABS);
}
static inline void LDA_ABS(p_cpu_info info)
{
	// AD
	lda_iml_d(info, D_ABS);
}
static inline void LDX_ABS(p_cpu_info info)
{
	// AE
	ldx_iml_d(info, D_ABS);
}
static inline void LAX_ABS(p_cpu_info info)
{
	// AF
	lax_iml_d(info, D_ABS);
}
// 0xB0-0xBF
static inline void BCS_REL(p_cpu_info info)
{
	// B0
	branch_true(info, FLAG_C);
}
static inline void LDA_IZY(p_cpu_info info)
{
	// B1
	lda_iml_d(info, D_IZY);
}
static inline void LAX_IZY(p_cpu_info info)
{
	// B3
	lax_iml_d(info, D_IZY);
}
static inline void LDY_ZPX(p_cpu_info info)
{
	// B4
	ldy_iml_d(info, D_ZPX);
}
static inline void LDA_ZPX(p_cpu_info info)
{
	// B5
	lda_iml_d(info, D_ZPX);
}
static inline void LDX_ZPY(p_cpu_info info)
{
	// B6
	ldx_iml_d(info, D_ZPY);
}
static inline void LAX_ZPY(p_cpu_info info)
{
	// B7
	lax_iml_d(info, D_ZPY);
}
static inline void CLV(p_cpu_info info)
{
	// B8
	IMP_ACC(info);
	reset_flag(info, FLAG_V);
}
static inline void LDA_ABY(p_cpu_info info)
{
	// B9
	lda_iml_d(info, D_ABY);
}
static inline void TSX(p_cpu_info info)
{
	// BA
	IMP_ACC(info);
	info->registers.X = info->registers.SP;
	set_n_z_flag(info, g_flag_nz_table[info->registers.X]);
}
static inline void LAS_ABY(p_cpu_info info)
{
	// BB
	word address = A_ABY(info);
	byte data = read_byte(info, address) & info->registers.SP;
	info->registers.A = info->registers.X = info->registers.SP = data;
	set_n_z_flag(info, g_flag_nz_table[info->registers.A]);
}
static inline void LDY_ABX(p_cpu_info info)
{
	// BC
	ldy_iml_d(info, D_ABX);
}
static inline void LDA_ABX(p_cpu_info info)
{
	// BD
	lda_iml_d(info, D_ABX);
}
static inline void LDX_ABY(p_cpu_info info)
{
	// BE
	ldx_iml_d(info, D_ABY);
}
static inline void LAX_ABY(p_cpu_info info)
{
	// BF
	lax_iml_d(info, D_ABY);
}
// 0xC0-0xCF
static inline void CPY_IMM(p_cpu_info info)
{
	// C0
	cpy_iml_d(info, IMM);
}
static inline void CMP_IZX(p_cpu_info info)
{
	// C1
	cmp_iml_d(info, D_IZX);
}
static inline void DCP_IZX(p_cpu_info info)
{
	// C3
	dcp_iml_a(info, A_IZX);
}
static inline void CPY_ZP(p_cpu_info info)
{
	// C4
	cpy_iml_d(info, D_ZP);
}
static inline void CMP_ZP(p_cpu_info info)
{
	// C5
	cmp_iml_d(info, D_ZP);
}
static inline void DEC_ZP(p_cpu_info info)
{
	// C6
	dec_iml_a_zp(info, A_ZP);
}
static inline void DCP_ZP(p_cpu_info info)
{
	// C7
	dcp_iml_a_zp(info, A_ZP);
}
static inline void INY(p_cpu_info info)
{
	// C8
	IMP_ACC(info);
	info->registers.Y += 1;
	set_n_z_flag(info, g_flag_nz_table[info->registers.Y]);
}
static inline void CMP_IMM(p_cpu_info info)
{
	// C9
	cmp_iml_d(info, IMM);
}
static inline void DEX(p_cpu_info info)
{
	// CA
	IMP_ACC(info);
	info->registers.X -= 1;
	set_n_z_flag(info, g_flag_nz_table[info->registers.X]);
}
static inline void AXS_IMM(p_cpu_info info)
{
	// CB
	// X:=A&X-#{imm}
	byte data = IMM(info);
	byte tmp = info->registers.A & info->registers.X;
	word tmp1 = tmp - data;
	info->registers.X = (byte)tmp1;
	set_flag(info, g_flag_nz_table[info->registers.X] | (tmp1 < 0x100));
}
static inline void CPY_ABS(p_cpu_info info)
{
	// CC
	cpy_iml_d(info, D_ABS);
}
static inline void CMP_ABS(p_cpu_info info)
{
	// CD
	cmp_iml_d(info, D_ABS);
}
static inline void DEC_ABS(p_cpu_info info)
{
	// CE
	dec_iml_a(info, A_ABS);
}
static inline void DCP_ABS(p_cpu_info info)
{
	// CF
	dcp_iml_a(info, A_ABS);
}
// 0xD0-0xDF
static inline void BNE_REL(p_cpu_info info)
{
	// D0
	branch_false(info, FLAG_Z);
}
static inline void CMP_IZY(p_cpu_info info)
{
	// D1
	cmp_iml_d(info, D_IZY);
}
static inline void DCP_IZY(p_cpu_info info)
{
	// D3
	dcp_iml_a(info, A_IZY);
}
static inline void CMP_ZPX(p_cpu_info info)
{
	// D5
	cmp_iml_d(info, D_ZPX);
}
static inline void DEC_ZPX(p_cpu_info info)
{
	// D6
	dec_iml_a_zp(info, A_ZPX);
}
static inline void DCP_ZPX(p_cpu_info info)
{
	// D7
	dcp_iml_a_zp(info, A_ZPX);
}
static inline void CLD(p_cpu_info info)
{
	// D8
	IMP_ACC(info);
	reset_flag(info, FLAG_D);
}
static inline void CMP_ABY(p_cpu_info info)
{
	// D9
	cmp_iml_d(info, D_ABY);
}
static inline void DCP_ABY(p_cpu_info info)
{
	// DB
	dcp_iml_a(info, A_ABY);
}
static inline void CMP_ABX(p_cpu_info info)
{
	// DD
	cmp_iml_d(info, D_ABX);
}
static inline void DEC_ABX(p_cpu_info info)
{
	// DE
	dec_iml_a(info, A_ABX);
}
static inline void DCP_ABX(p_cpu_info info)
{
	// DF
	dcp_iml_a(info, A_ABX);
}
// 0xE0-0xEF
static inline void CPX_IMM(p_cpu_info info)
{
	// E0
	cpx_iml_d(info, IMM);
}
static inline void SBC_IZX(p_cpu_info info)
{
	// E1
	sbc_iml_d(info, D_IZX);
}
static inline void ISC_IZX(p_cpu_info info)
{
	// E3
	isc_iml_a(info, A_IZX);
}
static inline void CPX_ZP(p_cpu_info info)
{
	// E4
	cpx_iml_d(info, D_ZP);
}
static inline void SBC_ZP(p_cpu_info info)
{
	// E5
	sbc_iml_d(info, D_ZP);
}
static inline void INC_ZP(p_cpu_info info)
{
	// E6
	inc_iml_a_zp(info, A_ZP);
}
static inline void ISC_ZP(p_cpu_info info)
{
	// E7
	isc_iml_a_zp(info, A_ZP);
}
static inline void INX(p_cpu_info info)
{
	// E8
	IMP_ACC(info);
	info->registers.X += 1;
	set_n_z_flag(info, g_flag_nz_table[info->registers.X]);
}
static inline void SBC_IMM(p_cpu_info info)
{
	// E9
	sbc_iml_d(info, IMM);
}
static inline void CPX_ABS(p_cpu_info info)
{
	// EC
	cpx_iml_d(info, D_ABS);
}
static inline void SBC_ABS(p_cpu_info info)
{
	// ED
	sbc_iml_d(info, D_ABS);
}
static inline void INC_ABS(p_cpu_info info)
{
	// EE
	inc_iml_a(info, A_ABS);
}
static inline void ISC_ABS(p_cpu_info info)
{
	// EF
	isc_iml_a(info, A_ABS);
}
// 0xF0-0xFF
static inline void BEQ_REL(p_cpu_info info)
{
	// F0
	branch_true(info, FLAG_Z);
}
static inline void SBC_IZY(p_cpu_info info)
{
	// F1
	sbc_iml_d(info, D_IZY);
}
static inline void ISC_IZY(p_cpu_info info)
{
	// F3
	isc_iml_a(info, A_IZY);
}
static inline void SBC_ZPX(p_cpu_info info)
{
	// F5
	sbc_iml_d(info, D_ZPX);
}
static inline void INC_ZPX(p_cpu_info info)
{
	// F6
	inc_iml_a_zp(info, A_ZPX);
}
static inline void ISC_ZPX(p_cpu_info info)
{
	// F7
	isc_iml_a_zp(info, A_ZPX);
}
static inline void SED(p_cpu_info info)
{
	// F8
	IMP_ACC(info);
	set_flag(info, FLAG_D);
}
static inline void SBC_ABY(p_cpu_info info)
{
	// F9
	sbc_iml_d(info, D_ABY);
}
static inline void ISC_ABY(p_cpu_info info)
{
	// FB
	isc_iml_a(info, A_ABY);
}
static inline void SBC_ABX(p_cpu_info info)
{
	// FD
	sbc_iml_d(info, D_ABX);
}
static inline void INC_ABX(p_cpu_info info)
{
	// FE
	inc_iml_a(info, A_ABX);
}
static inline void ISC_ABX(p_cpu_info info)
{
	// FF
	isc_iml_a(info, A_ABX);
}

// init
void init_opcodes(p_cpu_info info)
{
	// 0x00-0x0F
	info->opcodes[0x0] = BRK;
	info->opcodes[0x1] = ORA_IZX;
	info->opcodes[0x3] = SLO_IZX;
	info->opcodes[0x4] = NOP_ZP;
	info->opcodes[0x5] = ORA_ZP;
	info->opcodes[0x6] = ASL_ZP;
	info->opcodes[0x7] = SLO_ZP;
	info->opcodes[0x8] = PHP;
	info->opcodes[0x9] = ORA_IMM;
	info->opcodes[0xA] = ASL;
	info->opcodes[0xB] = ANC_IMM;
	info->opcodes[0xC] = NOP_ABS;
	info->opcodes[0xD] = ORA_ABS;
	info->opcodes[0xE] = ASL_ABS;
	info->opcodes[0xF] = SLO_ABS;

	// 0x10-0x1f
	info->opcodes[0x0] = BLP_REL;
	info->opcodes[0x1] = ORA_IZY;
	info->opcodes[0x3] = SLO_IZY;
	info->opcodes[0x4] = NOP_ZPX;
	info->opcodes[0x5] = ORA_ZPX;
	info->opcodes[0x6] = ASL_ZPX;
	info->opcodes[0x7] = SLO_ZPX;
	info->opcodes[0x8] = CLC;
	info->opcodes[0x9] = ORA_ABY;
	info->opcodes[0xA] = NOP;
	info->opcodes[0xB] = SLO_ABY;
	info->opcodes[0xC] = NOP_ABX;
	info->opcodes[0xD] = ORA_ABX;
	info->opcodes[0xE] = ASL_ABX;
	info->opcodes[0xF] = SLO_ABX;

	// 0x20-0x2F
	info->opcodes[0x0] = JSR_ABS;
	info->opcodes[0x1] = AND_IZX;
	info->opcodes[0x3] = RLA_IZX;
	info->opcodes[0x4] = BIT_ZP;
	info->opcodes[0x5] = AND_ZP;
	info->opcodes[0x6] = ROL_ZP;
	info->opcodes[0x7] = RLA_ZP;
	info->opcodes[0x8] = PLP;
	info->opcodes[0x9] = AND_IMM;
	info->opcodes[0xA] = ROL;
	info->opcodes[0xB] = ANC_IMM;
	info->opcodes[0xC] = BIT_ABS;
	info->opcodes[0xD] = AND_ABS;
	info->opcodes[0xE] = ROL_ABS;
	info->opcodes[0xF] = RLA_ABS;
}