/* thumb_calls.s */

.text
.globl main

.code 16
.align 2

ret_2:
	mov r0, #2
	bx lr /* no calls -> no push/pop lr -> return by bx lr */

fun:
	push {r4, lr}
	bl ret_2
	pop {r4, pc} /* a call present -> return by pop {..., pc} */


.code 32
.align 4

main:
	push {r4, lr}
	blx fun /* from arm to thumb so blx */
	pop {r4, lr}
	bx lr

