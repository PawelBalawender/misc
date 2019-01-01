/* printf.s */
.data

.balign 4
msg1: .asciz "Type a number: "

.balign 4
msg2: .asciz "%d times 5 is %d\n"

.balign 4
scan_fmt: .asciz "%d"

.balign 4
num_read: .word 0

.balign 4
ret: .word 0

.balign 4
ret2: .word 0


.text
.global main

main:
	ldr r1, ret_addr /* *ret_addt = lr */
	str lr, [r1]
	
	ldr r0, msg1_addr
	bl printf

	ldr r0, scan_fmt_addr
	ldr r1, num_read_addr
	bl scanf

	ldr r0, num_read_addr
	ldr r0, [r0]
	bl mult_by_5

	mov r2, r0
	ldr r1, num_read_addr
	ldr r1, [r1]
	ldr r0, msg2_addr
	bl printf

	ldr lr, ret_addr
	ldr lr, [lr]
	bx lr

mult_by_5:
	ldr r1, ret2_addr
	str lr, [r1]
	
	add r0, r0, r0, LSL #2

	ldr lr, ret2_addr
	ldr lr, [lr]
	bx lr

msg1_addr: .word msg1
msg2_addr: .word msg2
scan_fmt_addr: .word scan_fmt
num_read_addr: .word num_read
ret_addr: .word ret
ret2_addr: .word ret2

/* External */
.global printf
.global scanf

