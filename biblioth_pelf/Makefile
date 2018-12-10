CC=`which gcc`
LD=`which gcc`
RM=`which rm` -f
DOXYGEN=`which doxygen`
TAR=`which tar` -czvf

INCDIR=-I include

CFLAGS=-Wall -g -fpic $(INCDIR)
LFLAGS= -shared 

LIBDIR=build
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)

GARBAGE=*~ include/*~ *.orig src/*.orig include/*.orig include/*/*.orig *.o 

ARCHNAME=`date +%d-%m-%y-%H-%M`-`whoami`.tgz
ARCHCONTENT=$(SRC) *file include build README.txt make_mips_elf.c exemple.s exemple.o.ref

TARGET=build/libpelf.so

EXAMPLE_TARGET=exemple_elf

all : $(TARGET)


$(EXAMPLE_TARGET) : make_mips_elf.o build/libpelf.so
	$(LD) $^ -o $@

$(TARGET) : $(OBJ)
	$(LD) $(LFLAGS) $^ -o $@  

.c.o : 
	$(CC) $(CFLAGS) $< -c -o $(basename $<).o

clean : 
	$(RM) $(OBJ) $(GARBAGE) $(TARGET) $(EXAMPLE_TARGET)

documentation : 
	$(DOXYGEN)

archive : clean
	$(TAR) ../$(ARCHNAME) $(ARCHCONTENT) && mv ../$(ARCHNAME) .
