#ifndef _NES_UTILS_H__
#define _NES_UTILS_H__

typedef char sbyte;
typedef unsigned char ubyte;
typedef short sword;
typedef unsigned short uword;

#define __KB__ 1024
#define _4_KB_ (4*__KB__)
#define _8_KB_ (8*__KB__)
#define _16_KB_ (16*__KB__)

#define NES_SUCCESS 0

#define NES_NULL_POINT_ERROR -1
#define NES_ROM_NOT_FOUND_ERROR -2
#define NES_MEMORRY_ERROR -3
#define NES_ROM_READ_ERROR -4
#define NES_ROM_FORMAT_UNSUPPORT_ERROR -5
#define NES_ROM_VERSION_UNSUPPORT_ERROR -6

#define NES_MAPPER_PRG_COUNT_ERROR -7

#define NES_INIT_HARDWARE_ERROR -8

#define NES_MAPPER_NOT_FOUND_ERROR -9

#define DEBUG
#pragma warning(disable:4996)

#endif