/* factorial.s */
.data
msg1: 	.asciz "Type a number: "
msg2:	.asciz "The factorial of %d is %d\n"
fmt: 	.asciz "%d"


.text
.global main

main:
	str lr, [sp, #-4]!
	sub sp, sp, #4 /* make room for an int on the stack */

	ldr r0, msg1_addr
	bl printf

	ldr r0, fmt_addr
	mov r1, sp
	bl scanf

	ldr r0, [sp]
	bl factorial

	mov r2, r0
	ldr r1, [sp]
	ldr r0, msg2_addr
	bl printf

	add sp, sp, #+4 /* discard the int read by scanf */
	ldr lr, [sp], #+4
	bx lr

factorial:
	stmdb sp!, {r4, lr}
	/* the stack is not 8-byte aligned */
	mov r4, r0

	cmp r0, #0 /* if arg == 0: ret 1, else: branch */
	bne is_nonzero
	mov r0, #1
	b end

is_nonzero:
	sub r0, r0, #1
	bl factorial
	mov r1, r4
	mul r0, r0, r1

end:
	ldmia sp!, {r4, lr}
	bx lr

msg1_addr: 	.word msg1
msg2_addr:	.word msg2
fmt_addr:	.word fmt
