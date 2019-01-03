/* libbf.s */
.data

.align 1
arr: .byte 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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

gets:
	/* r0: &arr */
	push {r0, r4, r7, lr}
	
	mov r1, r0
	mov r0, #0 /* stdin fd: 0 */
	mov r2, #1 /* read 1 char */
	mov r7, #3 /* syscall 'read': 3 */
	swi #0

	pop {r0, r4, r7, lr}

arr_addr: .word arr

