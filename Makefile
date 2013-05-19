# Objects and source
SRC=main.c lisp.c lisp_values.c lisp_helper.c lisp_parser.c lisp_primitives.c
TARGET=lisp
OBJ=$(SRC:.c=.o)
DEBUG=-ggdb

# Executables
CC=gcc
RM=rm -f

# Pathes
INCDIR=
LIBDIR=

# Compiler and linker flags
CFLAGS=-Wall -Wunused -Werror $(DEBUG)
LDFLAGS=-lc $(DEBUG)

%.o : %.c
	$(CC) $(INCDIR) $(CFLAGS) -c $<

all : $(OBJ)
	$(CC) $(LIBDIR) $(LDFLAGS) $(OBJ) -o $(TARGET)

clean : 
	$(RM) *.o $(TARGET)
	
