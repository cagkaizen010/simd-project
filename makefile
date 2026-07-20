# Compilers and flags
CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm

ASM = nasm
ASMFLAGS = -f elf64

# Output executable name
TARGET = main 

# Object files needed to build the target
OBJS = main.o vector.o asmfunc1.o asmfunc2.o asmfunc3.o

# Default target
all: $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile main.c
main.o: main.c vector.h
	$(CC) $(CFLAGS) -c main.c

# Compile vector.c
vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c

asmfunc1.o: asmfunc1.asm
	$(ASM) $(ASMFLAGS) asmfunc1.asm -o asmfunc1.o

asmfunc2.o: asmfunc2.asm
	$(ASM) $(ASMFLAGS) asmfunc2.asm -o asmfunc2.o

asmfunc3.o: asmfunc3.asm
	$(ASM) $(ASMFLAGS) asmfunc3.asm -o asmfunc3.o

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)