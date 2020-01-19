#ifndef LOOKUP_TABLES_H
#define LOOKUP_TABLES_H

#include <uchar.h>
#include <stdint.h>

extern uint8_t sbox[256];
extern uint8_t inv_sbox[256];
extern unsigned char mul2[];
extern unsigned char mul3[];
extern unsigned char mul3[];
extern unsigned char mul9[];
extern unsigned char mul11[];
extern unsigned char mul13[];
extern unsigned char mul14[];
extern int rcon[10];

#endif
