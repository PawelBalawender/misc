/* call_arm_from_thumb.s */
.data
msg: .asciz "Hello, world #%d\n"

.text
.globl main

.code 16
.align 2
fun:
	push {r4, lr}
	mov r4, #0
	b loop_check

	loop:
		ldr r0, msg_addr
		mov r1, r4
		blx printf /* from thumb to arm, so blx; printf(msg, r4) */
		add r4, r4, #1 /* ++r4 */
	
	loop_check:
		cmp r4, #4
		blt loop
	
	pop {r4, pc}

.code 32
.align 4
main:
	push {r4, lr}
	blx fun
	pop {r4, lr}
	bx lr

msg_addr: .word msg

