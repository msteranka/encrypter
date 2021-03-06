#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "error.h"
#include "ciphers.h"
#define PATH_SIZE 256

#ifdef DEBUG
void print_hex(unsigned char *str, size_t len) 
{
	for(int i = 0; i < len; i++)
    	printf("%x", str[i]);
  	printf("\n");
}
#endif

void encrypt_block(unsigned char *str, unsigned char *key) 
{
	int rounds = 10;
  	// expanded_keys holds all of the subkeys generated by key_expansion()
  	unsigned char *expanded_keys = (unsigned char *) ec_malloc((rounds + 1) * BSIZE * sizeof(unsigned char));
  	unsigned char *subkey = (unsigned char *) ec_malloc((BSIZE + 1) * sizeof(unsigned char)); // subkey is the current round key being added
  	key_expansion(key, expanded_keys);

  	memcpy(subkey, expanded_keys, BSIZE);
	add_round_key(str, subkey); // Add round key

  	for(int i = 1; i < rounds; i++) {
  	  	sub_bytes(str); // Substitute bytes
  	  	shift_rows(str); // Shift rows
  	  	mix_columns(str); // Mix columns
  	  	memcpy(subkey, expanded_keys + (i * BSIZE), BSIZE); // Retrieve subkey
  	  	add_round_key(str, subkey); // Add round key
  	}

  	sub_bytes(str); // Substitute bytes
  	shift_rows(str); // Shift rows
  	memcpy(subkey, expanded_keys + (rounds * BSIZE), BSIZE); // Retrieve last key
  	add_round_key(str, subkey); // Add round key

  	free(expanded_keys);
  	free(subkey);
}

void decrypt_block(unsigned char *str, unsigned char *key) 
{
  	int rounds = 10;
  	unsigned char *expanded_keys = (unsigned char *) ec_malloc((rounds + 1) * BSIZE * sizeof(unsigned char)),
  	*subkey = (unsigned char *) ec_malloc((BSIZE + 1) * sizeof(unsigned char));
  	key_expansion(key, expanded_keys);

  	memcpy(subkey, expanded_keys + (rounds * BSIZE), BSIZE);
  	add_round_key(str, subkey); // Add round key
  	unshift_rows(str); // Shift rows
  	inv_sub_bytes(str); // Substitute bytes

  	for(int i = rounds - 1; i >= 1; i--) {
  	  	memcpy(subkey, expanded_keys + (i * BSIZE), BSIZE);
  	  	add_round_key(str, subkey);
  	  	inv_mix_columns(str);
  	  	unshift_rows(str);
  	  	inv_sub_bytes(str);
  	}

  	memcpy(subkey, expanded_keys, BSIZE);
  	add_round_key(str, subkey);

  	free(expanded_keys);
  	free(subkey);
}

void aes_encrypt_str(unsigned char *str, unsigned char *key, size_t str_len) {
  	unsigned char *block = (char *) ec_malloc(BSIZE * sizeof(unsigned char));

  	for(int i = 0; i < str_len; i += BSIZE) {
  	  	memcpy(block, str + i, BSIZE);
  	  	encrypt_block(block, key);
  	  	memcpy(str + i, block, BSIZE);
  	}

  	free(block);
}

void aes_decrypt_str(unsigned char *str, unsigned char *key, size_t str_len) 
{
  	unsigned char *block = (char *) ec_malloc(BSIZE * sizeof(unsigned char));

  	for(int i = 0; i < str_len; i += BSIZE) {
  	  	memcpy(block, str + i, BSIZE);
  	  	decrypt_block(block, key);
  	  	memcpy(str + i, block, BSIZE);
  	}

  	free(block);
}

void aes_encrypt_file(char *pathname, unsigned char *key) 
{
  	int fd = open(pathname, O_RDWR);
  	if(fd == -1) {
		fatal(NULL);	
	}
  	unsigned char *block = calloc(BSIZE + 1, sizeof(unsigned char));
  	if(block == NULL) {
		fatal(NULL);
	}
  	ssize_t nread;

  	while((nread = read(fd, block, BSIZE)) > 0) {
  	  	encrypt_block(block, key);
  	  	if(lseek(fd, nread * -1, SEEK_CUR) == -1) {
			fatal(NULL);	
		}
  	  	if(write(fd, block, BSIZE) == -1) {
			fatal(NULL);	
		}
  	  	memset(block, 0, BSIZE + 1);
  	}

  	if(nread == -1) {
		fatal(NULL);	
	}
  	if(close(fd) == -1) {
		fatal(NULL);	
	}
  	free(block);
}

void aes_decrypt_file(char *pathname, unsigned char *key) 
{
  	int fd = open(pathname, O_RDWR);
  	if(fd == -1) {
		fatal(NULL);	
	}
  	unsigned char *block = calloc(BSIZE + 1, sizeof(unsigned char));
  	if(block == NULL) {
		fatal(NULL);	
	}
  	ssize_t nread;

  	while((nread = read(fd, block, BSIZE)) > 0) {
  	  	decrypt_block(block, key);
  	  	if(lseek(fd, nread * -1, SEEK_CUR) == -1) {
			fatal(NULL);	
		}
  	  	if(write(fd, block, BSIZE) == -1) {
			fatal(NULL);	
		}
  	  	memset(block, 0, BSIZE + 1);
  	}

  	int i;
  	char c = 'A';
  	lseek(fd, BSIZE * -1, SEEK_END);
  	for(i = 0; c != '\0'; i++) {
		if(read(fd, &c, 1) == -1) {
			fatal(NULL);
		}
	}
  	off_t size = lseek(fd, 0, SEEK_END);
  	if (truncate(pathname, size - 16 + i - 1) == -1) {
		fatal(NULL);
	}

  	if(nread == -1) {
		fatal(NULL);	
	}
  	if(close(fd) == -1) {
		fatal(NULL);	
	}
  	free(block);
}

// encrypt_dir() encrypts a directory given by "path_name"
void aes_encrypt_dir(char *dir_path, unsigned char *key) 
{
	DIR *dir = opendir(dir_path); // Directory to be read
	if(dir == NULL) {
		fatal(NULL);	
	}
	struct dirent *file = readdir(dir); // Pointer to a dirent structure
	file = readdir(dir); // Call readdir again to skip . and ..

	while((file = readdir(dir)) != NULL) { // Read all files
		char *file_path = (char *) ec_malloc(PATH_SIZE);
		snprintf(file_path, PATH_SIZE, "%s/%s", dir_path, file->d_name);
		if(file->d_type == DT_DIR) {
			aes_encrypt_dir(file_path, key);	
		}
		else {
			aes_encrypt_file(file_path, key);	
		}
		free(file_path);
	}

	if(closedir(dir) == -1) {
		fatal(NULL);	
	}
}

// encrypt_dir() encrypts a directory given by "path_name"
void aes_decrypt_dir(char *dir_path, unsigned char *key) 
{
	DIR *dir = opendir(dir_path); // Directory to be read
	if(dir == NULL) {
		fatal(NULL);	
	}
	struct dirent *file = readdir(dir); // Pointer to a dirent structure
	file = readdir(dir); // Call readdir again to skip . and ..

	while((file = readdir(dir)) != NULL) { // Read all files
		char *file_path = (char *) ec_malloc(PATH_SIZE);
		snprintf(file_path, PATH_SIZE, "%s/%s", dir_path, file->d_name);
		if(file->d_type == DT_DIR) {
			aes_decrypt_dir(file_path, key);	
		}
		else {
			aes_decrypt_file(file_path, key);	
		}
		free(file_path);
	}

	if(closedir(dir) == -1) {
		fatal(NULL);	
	}
}
