#define __LIBRARY__

#include <linux/tty.h>
#include <linux/kernel.h>

extern void mem_init(long start, long end);

#define EXT_MEM_K (*(unsigned short *)0x90002)

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

void main(void)
{
	memory_end = (1<<20) + (EXT_MEM_K<<10);
	memory_end &= 0xfffff000;
	if (memory_end > 16*1024*1024)
		memory_end = 16*1024*1024;
	if (memory_end > 12*1024*1024)
		buffer_memory_end = 4*1024*1024;
	else if (memory_end > 6*1024*1024)
		buffer_memory_end = 2*1024*1024;
	else
		buffer_memory_end = 1*1024*1024;

	main_memory_start = buffer_memory_end;
	mem_init(main_memory_start, memory_end);

	tty_init();

	printk("memory start: %d, end: %d", main_memory_start, memory_end);

	__asm__ __volatile__(
			     "int $0x80\n\r"
			     "loop:\n\r"
			     "jmp loop"
			     ::);
}
