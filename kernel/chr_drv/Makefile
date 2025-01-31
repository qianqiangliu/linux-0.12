COMPILE_PREFIX ?= i386-jos-elf-

AR  := $(COMPILE_PREFIX)ar
LD  := $(COMPILE_PREFIX)ld
GCC := $(COMPILE_PREFIX)gcc
CCFLAG := -m32 -I../../include -nostdinc -ffreestanding -fno-pic -Wall -fomit-frame-pointer -fno-stack-protector -c
OBJS   := tty_io.o console.o

tty_io.o: tty_io.c
	$(GCC) $(CCFLAG) -o $@ $<

console.o: console.c
	$(GCC) $(CCFLAG) -o $@ $<

chr_drv.a: $(OBJS)
	$(AR) rcs $@ $^

clean:
	rm -f *.o
	rm -f chr_drv.a
