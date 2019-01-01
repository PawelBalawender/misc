/* array.s */

.data
.balign 4
a: .skip 400
.balign 4
b: .skip 8

.text
.global main

main:
	ldr r1, addr_a
	mov r2, #0

loop:
	cmp r2, #100 /* if r2 == 100: break */
	beq end
	add r3, r1, r2, LSL #2 /* a[i++] = i */
	str r2, [r3]
	add r2, r2, #1
	b loop

end:
	bx lr

addr_a: .word a

