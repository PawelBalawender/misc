/* mul64.s */
.data

.align 4
msg:	.asciz "Multiplication of %lld by %lld: %lld\n"

.align 8
a_low:	.word 3755744309
a_high:	.word 2

.align 8
b_low:	.word 12345678
b_high:	.word 0


.text
.global main

main:
	push {r4, r5, r6, r7, r8, lr}

	ldr r4, a_low_addr
	ldr r5, [r4, #4]
	ldr r4, [r4]

	ldr r6, b_low_addr
	ldr r7, [r6, #4]
	ldr r6, [r6]

	mov r0, r4
	mov r1, r5
	mov r2, r6
	mov r3, r7
	bl mul64	/* mul64(a_low, a_high, b_low, b_high) */

	push {r1}
	push {r0}
	push {r7}
	push {r6}
	mov r3, r5
	mov r2, r4
	ldr r0, msg_addr
	bl printf	/* printf(msg, a, b, mul64(a, b)) */

	add sp, sp, #16
	mov r0, #0
	pop {r4, r5, r6, r7, r8, lr}
	bx lr

mul64:
	push {r4, r5, r6, r7, r8, lr}
	mov r4, r0
	mov r5, r1

	umull r0, r6, r2, r4
	umull r7, r8, r3, r4
	umull r4, r5, r2, r5
	adds r2, r7, r4
	adc r1, r2, r6

	pop {r4, r5, r6, r7, r8, lr}
	bx lr

msg_addr: .word msg
a_low_addr: .word a_low
a_high_addr:.word a_high
b_low_addr: .word b_low
b_high_addr:.word b_high

