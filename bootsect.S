SETUPLEN  = 4
BOOTSEG   = 0x7c0
INITSEG   = 0x9000
SETUPSEG  = 0x9020
ROOT_DEV  = 0x000

.code16
.text
.global _start

_start:
	movw $BOOTSEG, %ax
	movw %ax, %ds
	movw $INITSEG, %ax
	movw %ax, %es
	movw $256, %cx
	subw %si, %si
	subw %di, %di

	rep
	movsw

	call clear_screen # 调用清屏函数

	jmpl $INITSEG, $go

go:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0xff00, %sp

load_setup:
	movw $0x0000, %dx
	movw $0x0002, %cx
	movw $0x0200, %bx
	movb $SETUPLEN, %al
	movb $0x02, %ah
	int  $0x13
	jnc  ok_load_setup
	movw $0x0000, %dx
	movw $0x0000, %ax
	int  $0x13
	jmp  load_setup

ok_load_setup:
	movw $msg, %ax
	movw %ax, %bp
	movw $0x01301, %ax
	movw $0x0c, %bx
	movw $33, %cx
	movw $0, %dx
	int  $0x10

	jmpl $SETUPSEG, $0

loop:
	jmp loop

clear_screen:
	movw $0x0600, %ax # ah = 0x06（功能号），al = 0x00（清屏）
	movw $0x0700, %bx # bh = 0x07（属性字节：黑底白字）
	movw $0x0000, %cx # ch = 0（起始行），cl = 0（起始列）
	movw $0x184f, %dx # dh = 24（结束行），dl = 79（结束列）
	int $0x10         # 调用 BIOS 中断 0x10
	ret

msg:
	.ascii "bootsect.S: setup has been loaded"

.org 508
root_dev:
	.word ROOT_DEV
boot_flag:
	.word 0xaa55
