#include <stdio.h>
#include <stdlib.h>

void fatal(char *msg) 
{
	if (msg == NULL) {
		perror("ERROR");
	} else {
  		fprintf(stderr, "%s", msg);
	}
  	exit(EXIT_FAILURE);
}

void *ec_malloc(size_t size) 
{
  	void *ptr = malloc(size);
  	if(ptr == NULL) {
		fatal("Could not allocate memory");	
	}
  	return ptr;
}
