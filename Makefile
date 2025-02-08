AS := $(COMPILE_PREFIX)as --32
LD := $(COMPILE_PREFIX)ld -m elf_i386

LDFLAGS := -Ttext 0x0 -s --oformat binary

image: linux.img

linux.img: tools/build bootsect setup kernel/system
	./tools/build bootsect setup kernel/system > $@

tools/build: tools/build.c
	gcc -o $@ $<

kernel/system: kernel/head.S kernel/*.c
	cd kernel; make system; cd ..

bootsect: bootsect.o
	$(LD) $(LDFLAGS) -o $@ $<

bootsect.o: bootsect.S
	$(AS) -o $@ $<

setup: setup.o
	$(LD) $(LDFLAGS) -e _start_setup -o $@ $<

setup.o: setup.S
	$(AS) -o $@ $<

run: linux.img
	qemu-system-i386 -boot a -fda linux.img

clean:
	rm -f *.o
	rm -f bootsect
	rm -f setup
	rm -f tools/build
	rm -f linux.img
	cd kernel; make clean; cd ..
	cd lib; make clean; cd ..
