/* asm.s */

.data
.balign 4
foo:
	.word 0
.balign 4
bar:
	.word 0

.text
.balign 4
.global main
main:
	ldr r1, foo_addr
	mov r3, #3
	str r3, [r1]
	
	ldr r2, bar_addr
	mov r3, #4
	str r3, [r2]
	
	ldr r1, foo_addr
	ldr r1, [r1]
	ldr r2, bar_addr
	ldr r2, [r2]
	
	add r0, r1, r2
	bx lr		/* Return from main */

foo_addr: .word foo
bar_addr: .word bar

