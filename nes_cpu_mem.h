#ifndef __NES_CPU_MEM_
#define __NES_CPU_MEM_


byte zp_read_byte(p_cpu_info info, byte address);
void zp_write_byte(p_cpu_info info, byte address, byte value);
word zp_read_word(p_cpu_info info, byte address);
byte read_byte(p_cpu_info info, word address);
void write_byte(p_cpu_info info, word address, byte value);
word read_word(p_cpu_info info, word address);
void write_word(p_cpu_info info, word address, word value);
void push_byte(p_cpu_info info, byte value);
byte pop_byte(p_cpu_info info);
void push_word(p_cpu_info info, word value);
word pop_word(p_cpu_info info);

#endif