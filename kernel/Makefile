COMPILE_PREFIX ?= i386-jos-elf-

GCC := $(COMPILE_PREFIX)gcc
LD := $(COMPILE_PREFIX)ld
CCFLAG := -mcpu=i386 -I../include -nostdinc -Wall -fomit-frame-pointer -c
LDFLAG := -Ttext 0x0 -s --oformat binary
OBJS := head.o

system: $(OBJS)
	$(LD) $(LDFLAG) -e startup_32 -o $@ $^

head.o: head.S
	$(GCC) -traditional -c -o $@ $<

clean:
	rm -f *.o
	rm -f system
