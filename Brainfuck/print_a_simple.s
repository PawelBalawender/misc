.data
.align 1
arr: .byte 0, 0, 0, 0
after:
.set arr_len, after - arr

.text
puts:
	/* r0: &arr */
	push {r0, r4, r7, lr}

	mov r1, r0
	mov r0, #1 /* stdout file descriptor: 1 */
	mov r2, #1 /* output 1 char */
	mov r7, #4 /* system call 'write': 4 */
	swi #0
	
	pop {r0, r4, r7, lr}	
	bx lr

arr_addr: .word arr

.globl main
main:
	push {r4, lr}
	ldr r0, arr_addr

	ldrb r1, [r0]
	add r1, r1, #4
	strb r1, [r0]

loop:
	ldrb r1, [r0]
	cmp r1, #0
	beq end

	add r0, r0, #1
	ldrb r1, [r0]
	add r1, r1, #24
	strb r1, [r0]
	
	sub r0, r0, #1
	ldrb r1, [r0]
	sub r1, r1, #1
	strb r1, [r0]
	b loop


end:
	add r0, r0, #1
	ldrb r1, [r0]
	add r1, r1, #1
	strb r1, [r0]
	
	bl puts

	pop {r4, lr}
	bx lr

