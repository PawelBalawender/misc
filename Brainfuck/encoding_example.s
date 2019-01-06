.text
.globl main
main:
	push {r0, r4, lr, pc}
	pop {r0, r4, lr, pc}
	push {r4, pc}
	pop {r4, pc}
	ldr r0, [r0]
	ldr r1, [r0]
	str r1, [r0]
	str r0, [r0]

	/* 1101 */
	mov r0, #0
	mov r0, r0
	mov r0, r1
	mov r1, r0
	mov r0, #127
	mov r0, #128
	mov r1, #0
	mov r0, #1
	mov r1, #1
	mov r2, #1
	mov r3, #1
	/* 0100 */
	add r0, r0, #1
	add r0, r0, #2
	add r0, r0, #128
	add r0, r1, #1
	add r0, r2, #1
	add r0, r3, #1
	add r1, r0, #0
	add r2, r0, #0
	add r2, r0, #0
	/* 0010 */
	sub r0, r0, #1
	/* 1xxx */
	bl main

	bx lr

