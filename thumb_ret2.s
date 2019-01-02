/* thumb_ret2.s */

.text
.globl main


.code 32

main:
	push {r4, lr}
	blx fun
	pop {r4, lr}
	bx lr


.code 16
.align 2

fun:
	mov r0, #2
	bx lr

