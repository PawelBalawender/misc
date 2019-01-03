/* hello_brainfuck_like.s */
.data

.align 4
arr: .byte 72, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 10, 0
after:
.set arr_len, after - arr


.text
.globl main

main:
	push {r4, lr}
	bl puts
	pop {r4, lr}
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
	mov pc, lr

arr_addr: .word arr

