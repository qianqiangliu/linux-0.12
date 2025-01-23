.code16
.text
.globl _start_setup

_start_setup:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es

	movw $setup_msg, %ax
	movw %ax, %bp
	movw $0x1301, %ax
	movw $0x0c, %bx
	movw $25, %cx
	movb $2, %dh
	movb $0, %dl
	int $0x10

loop:
	jmp loop

setup_msg:
	.ascii "setup.S: setup is running"
