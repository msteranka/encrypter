#ifndef CIPHERS_H
#define CIPHERS_H
#define BSIZE 16

void key_expansion(unsigned char *input_key, unsigned char *expanded_keys);
void sub_bytes(unsigned char *state);
void inv_sub_bytes(unsigned char *state);
void shift_rows(unsigned char *state);
void unshift_rows(unsigned char *state);
void mix_columns(unsigned char *state);
void inv_mix_columns(unsigned char *state);
void add_round_key(unsigned char *state, unsigned char *key);

#endif
