COMPILE_PREFIX ?= i386-jos-elf-

AS := $(COMPILE_PREFIX)as --32
LD := $(COMPILE_PREFIX)ld -m elf_i386

LDFLAGS := -Ttext 0x0 -s --oformat binary

image: linux.img

linux.img: bootsect.o
	$(LD) $(LDFLAGS) -o $@ $<

bootsect.o: bootsect.S
	$(AS) -o $@ $<

run: linux.img
	qemu-system-i386 -boot a -fda linux.img

clean:
	rm -f *.o
	rm -f linux.img
