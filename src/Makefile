IDIR=../include
ODIR=../obj
BDIR=../bin
CC=gcc
CFLAGS=-I$(IDIR) -O
TARGET=aes

_DEPS=aes.h ciphers.h lookuptables.h error.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
 
_OBJ=main.o aes.o ciphers.o lookuptables.o error.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<

aes: $(OBJ)
	$(CC) $(FLAGS) -o $(BDIR)/$@ $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
