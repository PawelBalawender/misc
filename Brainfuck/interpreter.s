/* hello_brainfuck_like.s */
.data

.align 1
arr: .byte 0, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 10, 0
after:
.set arr_len, after - arr


.text
.globl main

main:
	push {r4, lr}
	ldr r0, arr_addr

	bl gets
	bl puts

	pop {r4, lr}
	bx lr

inc:
	/* r0: &arr */
	ldrb r1, [r0]
	add r1, r1, #1
	strb r1, [r0]
	bx lr

dec:
	/* r0: &arr */
	ldrb r1, [r0]
	sub r1, r1, #1
	strb r1, [r0]
	bx lr

pntr_inc:
	/* r0: &arr */
	add r0, r0, #1
	bx lr

pntr_dec:
	/* r0: &arr */
	sub r0, r0, #1
	bx lr

puts:
	/* r0: &arr */
	push {r7, lr}
	
	mov r0, #1 /* stdout file descriptor: 1 */
	ldr r1, arr_addr
	mov r2, #arr_len
	mov r7, #4 /* system call 'write': 4 */
	swi #0
	
	pop {r7, lr}
	bx lr

gets:
	/* r0: &arr */
	push {r0, r1, r7, lr}
	
	mov r1, r0
	mov r0, #0 /* stdin fd: 0 */
	mov r2, #1 /* read 1 char */
	mov r7, #3 /* read syscall: 3 */
	swi #0

	pop {r0, r1, r7, lr}
	bx lr

arr_addr: .word arr

