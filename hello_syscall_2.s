/* hello_syscall_2.s */
.data

msg: .asciz "Hello, world!\n"
after_msg:
.set len, after_msg - msg

.text
.globl main

main:
	push {r7, lr}

	mov r0, #1 /* stdout file descriptor: 1 */
	ldr r1, msg_addr
	mov r2, #len
	mov r7, #4 /* system call 'write': 4 */
	swi #0 /* make a linux syscall: syscall operand=0 always */
	
	mov r0, #0
	pop {r7, lr}
	bx lr

msg_addr: .word msg
