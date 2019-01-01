.data

greeting:
	.asciz "Hello, world!"

.balign 4
return: .word 0

.text
.global main

main:
	ldr r1, ret_addr
	str lr, [r1]
	ldr r0, greet_addr

	bl puts

	ldr r1, ret_addr
	ldr lr, [r1]
	bx lr

ret_addr: .word return
greet_addr: .word greeting

