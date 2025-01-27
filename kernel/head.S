.code32
.text
.global startup_32
startup_32:
	movl $0x10, %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs

	movl $0x18, %eax
	movw %ax, %gs

	call setup_idt
	int $0x80

	movl $0x18, %eax
	movw %ax, %gs
	movl $0x0, %edi
	movb $0xf, %ah
	movb $0x42, %al
	movw %ax, %gs:(%edi)

loop:
	jmp loop

setup_idt:
	leal ignore_int, %edx
	movl $0x00080000, %eax
	movw %dx, %ax
	movw $0x8e00, %dx
	leal idt, %edi
	movl $256, %ecx
rp_sidt:
	movl %eax, (%edi)
	movl %edx, 4(%edi)
	addl $8, %edi
	decl %ecx
	jne  rp_sidt
	lidt idt_descr
	ret

ignore_int:
	/* we do not have function _printk now, so trick it */
	pushl %eax
	pushl %ecx
	pushl %edx
	pushw %ds
	pushw %es
	pushw %fs
	movl  $0x10, %eax
	movw  %ax, %ds
	movw  %ax, %es
	movw  %ax, %fs
	/* call  _printk */
	movl  $0x96, %edi
	movb  $'I', %al
	movb  $0x0c, %ah
	movw  %ax, %gs:(%edi)
	popw  %fs
	popw  %es
	popw  %ds
	popl  %edx
	popl  %ecx
	popl  %eax
	iret

.align 4
.word 0
idt_descr:
	.word 256 * 8 - 1
	.long idt

idt:
	.fill 256, 8, 0
