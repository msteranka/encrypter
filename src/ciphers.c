#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <stdint.h>
#include "lookuptables.h"
#include "error.h"

static void key_expansion_core(unsigned char *in, unsigned char i) 
{
  	unsigned int *q = (unsigned int *) in;

  	*q = (*q << 8) | ((*q & 0xff) << 24); // Rotate left all bytes

  	for(int i = 0; i < 4; i++) { // Sub each byte using Sbox
		in[i] = sbox[in[i]];
	}

  	in[0] ^= rcon[i]; // Rcon
}

void key_expansion(unsigned char *input_key, unsigned char *expanded_keys) 
{
  	for(int i = 0; i < 16; i++) {
  	  	expanded_keys[i] = input_key[i];
	}

  	int bytes_generated = 16;
  	int rcon_iteration = 1;
  	unsigned char temp[4];

  	while(bytes_generated < 176) {
  	  	for(int i = 0; i < 4; i++) {
  	  	  	temp[i] = expanded_keys[i + bytes_generated - 4];
		}

  	  	if(bytes_generated % 16 == 0) {
  	  	  	key_expansion_core(temp, rcon_iteration++);
		}

  	  	for(unsigned char a = 0; a < 4; a++) {
  	  	  	expanded_keys[bytes_generated] = expanded_keys[bytes_generated - 16] ^ temp[a];
  	  	  	bytes_generated++;
  	  	}
  	}
}

void sub_bytes(unsigned char *state) 
{
  	for(int i = 0; i < 16; i++) {
  	  	state[i] = sbox[state[i]];
	}
}

void inv_sub_bytes(unsigned char *state) 
{
  	for(int i = 0; i < 16; i++) {
  	  	state[i] = inv_sbox[state[i]];
	}
}

void shift_rows(unsigned char *state) 
{
  	unsigned char *tmp = (unsigned char *) ec_malloc(16);

  	tmp[0] = state[0];
  	tmp[1] = state[5];
  	tmp[2] = state[10];
  	tmp[3] = state[15];

  	tmp[4] = state[4];
  	tmp[5] = state[9];
  	tmp[6] = state[14];
  	tmp[7] = state[3];

  	tmp[8] = state[8];
  	tmp[9] = state[13];
  	tmp[10] = state[2];
  	tmp[11] = state[7];

  	tmp[12] = state[12];
  	tmp[13] = state[1];
  	tmp[14] = state[6];
  	tmp[15] = state[11];

  	for(int i = 0; i < 16; i++) {
  	  	state[i] = tmp[i];
	}

  	free(tmp);
}

void unshift_rows(unsigned char *state) 
{
  	unsigned char *tmp = (unsigned char *) ec_malloc(16);

  	tmp[0] = state[0];
  	tmp[5] = state[1];
  	tmp[10] = state[2];
  	tmp[15] = state[3];

  	tmp[4] = state[4];
  	tmp[9] = state[5];
  	tmp[14] = state[6];
  	tmp[3] = state[7];

  	tmp[8] = state[8];
  	tmp[13] = state[9];
  	tmp[2] = state[10];
  	tmp[7] = state[11];

  	tmp[12] = state[12];
  	tmp[1] = state[13];
  	tmp[6] = state[14];
  	tmp[11] = state[15];

  	for(int i = 0; i < 16; i++) {
  	  	state[i] = tmp[i];
	}

  	free(tmp);
}

void mix_columns(unsigned char *state) 
{
  	unsigned char *tmp = (unsigned char *) ec_malloc(16);

  	for(int i = 0; i < 16; i += 4) {
  	  	tmp[i] = (unsigned char) (mul2[state[i]] ^ mul3[state[i + 1]] ^ state[i + 2] ^ state[i + 3]);
  	  	tmp[i + 1] = (unsigned char) (state[i] ^ mul2[state[i + 1]] ^ mul3[state[i + 2]] ^ state[i + 3]);
  	  	tmp[i + 2] = (unsigned char) (state[i] ^ state[i + 1] ^ mul2[state[i + 2]] ^ mul3[state[i + 3]]);
  	  	tmp[i + 3] = (unsigned char) (mul3[state[i]] ^ state[i + 1] ^ state[i + 2] ^ mul2[state[i + 3]]);
  	}

  	for(int i = 0; i < 16; i++) {
  	  	state[i] = tmp[i];
	}

  	free(tmp);
}

void inv_mix_columns(unsigned char *state) 
{
  	unsigned char *tmp = (unsigned char *) ec_malloc(16);

  	for(int i = 0; i < 16; i += 4) {
  	  	tmp[i] = (unsigned char) (mul14[state[i]] ^ mul11[state[i + 1]] ^ mul13[state[i + 2]] ^ mul9[state[i + 3]]);
  	  	tmp[i + 1] = (unsigned char) (mul9[state[i]] ^ mul14[state[i + 1]] ^ mul11[state[i + 2]] ^ mul13[state[i + 3]]);
  	  	tmp[i + 2] = (unsigned char) (mul13[state[i]] ^ mul9[state[i + 1]] ^ mul14[state[i + 2]] ^ mul11[state[i + 3]]);
  	  	tmp[i + 3] = (unsigned char) (mul11[state[i]] ^ mul13[state[i + 1]] ^ mul9[state[i + 2]] ^ mul14[state[i + 3]]);
  	}

  	for(int i = 0; i < 16; i++) {
  	  	state[i] = tmp[i];
	}

  	free(tmp);
}

void add_round_key(unsigned char *state, unsigned char *key) 
{
  	for(int i = 0; i < 16; i++) {
  	  	state[i] ^= key[i];
	}
}
