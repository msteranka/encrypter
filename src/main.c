#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "aes.h"
#include "error.h"

int main(int argc, char *argv[]) 
{
	if (argc != 4) {
		fatal("usage: <e/d> <dir/file> <key>\n");
	}

	if (strlen(argv[3]) != 16) {
		fatal("Key must be 16 characters long\n");
	}

	struct stat buf;
	if(stat(argv[2], &buf) != 0) {
		fatal(NULL);
	}

	if (S_ISREG(buf.st_mode)) {
		if (argv[1][0] == 'e') {
			aes_encrypt_file(argv[2], argv[3]);
		} else {
			aes_decrypt_file(argv[2], argv[3]);
		}
	} else if(S_ISDIR(buf.st_mode)) {
		if (argv[1][0] == 'e') {
			aes_encrypt_dir(argv[2], argv[3]);
		} else {
			aes_decrypt_dir(argv[2], argv[3]);
		}
	} else {
		fatal("Invalid file type\n");
	}

  	return EXIT_SUCCESS;
}
