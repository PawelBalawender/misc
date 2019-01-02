/* collatz.s; does collatz conjecture apply to a given number? */
.data

msg:	.asciz "Type a number: "
msg2:	.asciz "Hailstone length for %d: %d\n"
scn_fmt:.asciz "%d"

.text
.global main

main:
	push {lr}
	sub sp, sp, #4
	
	ldr r0, msg_addr
	bl printf	/* printf(msg) */

	ldr r0, scn_fmt_addr
	mov r1, sp
	bl scanf	/* scanf(r0, &x = sp) */

	ldr r0, [sp]
	bl collatz /* collatz(x = *sp) */

	mov r2, r0
	ldr r1, [sp]
	ldr r0, msg2_addr
	bl printf /* printf(msg2, x, collatz(x)) */

	add sp, sp, #4
	pop {lr}
	bx lr

collatz:
	mov r1, r0
	mov r0, #0

collatz_loop:
	cmp r1, #1
	beq collatz_end
	and r2, r1, #1
	cmp r2, #0
	bne collatz_odd

collatz_even:
	mov r1, r1, ASR #1
	b collatz_end_loop

collatz_odd:
	add r1, r1, r1, LSL #1
	add r1, r1, #1

collatz_end_loop:
	add r0, r0, #1
	b collatz_loop

collatz_end:
	bx lr

msg_addr:		.word msg
msg2_addr:		.word msg2
scn_fmt_addr:	.word scn_fmt

