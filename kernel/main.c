#define __LIBRARY__

#include <linux/tty.h>
#include <linux/kernel.h>

void main(void)
{
	tty_init();

	for (int i = 0; i < 25; i++) {
		printk("this is line %d\n\r", i);
	}

	__asm__ __volatile__(
		"int $0x80\n\r"
		"loop:\n\r"
		"jmp loop"
		::);
}
