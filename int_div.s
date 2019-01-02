/* int_div.s */

.text
.globl main

main:
	mov r0, #100
	mov r1, #3
	b unsigned_div
	bx lr

unsigned_div:
	/* N = D*Q + R */
	/* r0: N, r1: D */
	mov r2, r1
	mov r1, r0
	mov r0, #0

	b .Lloop_check
	
	.Lloop:
		add r0, r0, #1 /* Q += 1 */
		sub r1, r1, r2 /* N -= D */

	.Lloop_check:
		cmp r1, r2
		bhs .Lloop  /* if N >= D: branch */
	
	bx lr

