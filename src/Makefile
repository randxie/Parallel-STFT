all: runSTFT mpirunSTFT

runSTFT:
# Make file
EXEC1 := runSTFT
SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.o,$(SRC))
# set options
include Makefile_options.inc
# Rules
$(EXEC1): $(OBJ)
$(OBJ): runSTFT.cc

mpirunSTFT:
# Make file
EXEC1 := mpirunSTFT
SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.o,$(SRC))
# set options
include Makefile_options.inc
# Rules
$(EXEC1): $(OBJ)
$(OBJ): mpirunSTFT.cc


