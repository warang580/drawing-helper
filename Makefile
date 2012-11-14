# Makefile "Drawing Helper"

# Program name
EXEC = gdh

# Compiler
CC = gcc

# Compiler options
CFLAGS ?= -W -Wall -Wextra -Wmissing-declarations -Wmissing-prototypes \
          -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual \
          -g

# Debug mode
CFLAGS += -D DEBUG

# Linker options
LDFLAGS ?=

# GTK+
CFLAGS  += `pkg-config --cflags gtk+-2.0`
LDFLAGS += `pkg-config --libs gtk+-2.0`

# Maths library
CFLAGS += -lm

# Source files
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:%=%.d)

# Linking
all : info $(EXEC) endinfo

# Compilation
$(EXEC) : $(OBJ)
	@ echo "\t★ $@  "
	@ $(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

%.o : %.c
	@ echo "\t★ $@  "
	@ $(CC) -c $< -o $@ -MT $@ -MD -MP -MF $@.d $(CFLAGS)

# These rules will always be executable
.PHONY : clean again mrproper info endinfo archive

again : clean all

# Informations
info :
	@echo "--------------- Drawing Helper ---------------"
	@echo "Compiling :"

endinfo :
	@echo "\t✔ ... OK ! "
	@echo "----------------------------------------------"

# Cleaning routines
clean :
	@ rm -f $(OBJ)
	@ rm -f $(DEP)
	@ rm -f *~
	@ rm -f src/*~

mrproper : clean
	@ rm -f $(EXEC)
	@ rm -f $(EXEC)_files.tgz

archive : mrproper
	@ tar -cvzf $(EXEC)_files.tgz *

-include $(DEP)
