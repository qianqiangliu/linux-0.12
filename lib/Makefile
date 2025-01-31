COMPILE_PREFIX ?= i386-jos-elf-

AR  := $(COMPILE_PREFIX)ar
LD  := $(COMPILE_PREFIX)ld
GCC := $(COMPILE_PREFIX)gcc
CCFLAG := -m32 -I../include -nostdinc -ffreestanding -fno-pic -Wall -fomit-frame-pointer -fno-stack-protector -c
OBJS   := string.o

lib.a: $(OBJS)
	$(AR) rcs $@ $^

string.o: string.c
	$(GCC) $(CCFLAG) -o $@ $<

clean: 
	rm -f *.o
	rm -f lib.a
