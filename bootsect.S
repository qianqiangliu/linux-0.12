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

	movw $msg, %ax
	movw %ax, %bp
	movw $0x1301, %ax
	movw $0x0c, %bx # bh = 0，bl = 0c（红色）
	movw $12, %cx   # cx = 串长度
	movw $0x00, %dx # dh = 行号（0 表示第一行），dl = 列号（0 表示第一列）
	int $0x10       # 10h 号中断，通知显卡刷新内容

loop:
	jmp loop

msg:
	.ascii "Hello World!"

.org 510
boot_flag:
	.word 0xaa55
