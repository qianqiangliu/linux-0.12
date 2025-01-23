BOOTSEG = 0x7c0

.code16
.text
.global _start

_start:
	jmpl $BOOTSEG, $start2

start2:
	movw $BOOTSEG, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	call clear_screen # 调用清屏函数

	movw $msg, %ax
	movw %ax, %bp
	movw $0x1301, %ax
	movw $0x0c, %bx # bh = 0，bl = 0c（红色）
	movw $12, %cx   # cx = 串长度
	movw $0x00, %dx # dh = 行号（0 表示第一行），dl = 列号（0 表示第一列）
	int $0x10       # 10h 号中断，通知显卡刷新内容

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
	.ascii "Hello World!"

.org 510
boot_flag:
	.word 0xaa55
