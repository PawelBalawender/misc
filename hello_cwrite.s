/* hello_cwrite.s */
.data

msg: .asciz "Hello, world!\n"
after_msg:
.set msg_size, after_msg - msg

.text
.globl main

main:
	push {r4, lr}
	mov r0, #1 
	ldr r1, msg_addr
	mov r2, #msg_size
	bl write

	mov r0, #0
	pop {r4, lr}
	bx lr

msg_addr: .word msg

