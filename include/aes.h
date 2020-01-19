#ifndef AES_H
#define AES_H

void aes_encrypt_str(char *str, char *key, size_t len, size_t block_size);
void aes_decrypt_str(char *str, char *key, size_t len, size_t block_size);
void aes_encrypt_file(char *pathname, unsigned char *key);
void aes_decrypt_file(char *pathname, unsigned char *key);
void aes_encrypt_dir(char *dir_path, unsigned char *key);
void aes_decrypt_dir(char *dir_path, unsigned char *key);

#endif
