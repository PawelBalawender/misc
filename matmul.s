/* matmul.s */

.data
mat_A: .float 0.1, 0.2, 0.0, 0.1
       .float 0.2, 0.1, 0.3, 0.0
       .float 0.0, 0.3, 0.1, 0.5 
       .float 0.0, 0.6, 0.4, 0.1
mat_B: .float  4.92,  2.54, -0.63, -1.75
       .float  3.02, -1.51, -0.87,  1.35
       .float -4.29,  2.14,  0.71,  0.71
       .float -0.95,  0.48,  2.38, -0.95
mat_C: .float 0.0, 0.0, 0.0, 0.0
       .float 0.0, 0.0, 0.0, 0.0
       .float 0.0, 0.0, 0.0, 0.0
       .float 0.0, 0.0, 0.0, 0.0
       .float 0.0, 0.0, 0.0, 0.0

format_result : .asciz "Matrix result is:\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n"

.text
.globl main

main:
	push {r4, r5, r6, lr}
	vpush {d0-d1}

	ldr r0, addr_mat_A
	ldr r1, addr_mat_B
	ldr r2, addr_mat_C
	bl naive_matmul_4x4 /* naive_matmul(A, B, C) */

	ldr r4, addr_mat_C
	vldr s0, [r4] /* s0 = c[0][0] */
	vcvt.f64.f32 d1, s0
	vmov r2, r3, d1 /* {r2, r3} = f64 d1 = (f64)(f32 s0) */
	
	mov r6, sp
	
	mov r5, #1 /* iter from 1 cause c[0][0] has already been handled */
	add r4, r4, #60 /* goto c[3][3] */
	.Lloop:
		vldr s0, [r4]
		vcvt.f64.f32 d1, s0 /* f64 d1 = (f64)(f32 s0 = *r4) */

		sub sp, sp, #8
		vstr d1, [sp]
		sub r4, r4, #4
		add r5, r5, #1
		cmp r5, #16
		bne .Lloop
	
	ldr r0, addr_format_result
	bl printf
	
	mov sp, r6
	mov r0, #0
	vpop {d0-d1}
	pop {r4, r5, r6, lr}
	bx lr

naive_matmul_4x4:
	push {r4, r5, r6, r7, r8, lr}
	mov r4, r2
	mov r5, #16
	mov r6, #0
	
	b .Lloop_init_test

	.Lloop_init:
		str r6, [r4], #4 /* *(r4++) = r6 */
	
	.Lloop_init_test:
		subs r5, r5, #1
		bge .Lloop_init
	
	mov r4, #0

	/* r4: i	r5: j	r6: k */
	.Lloop_i:
		cmp r4, #4
		beq .Lend_loop_i
		
		mov r5, #0

		.Lloop_j:
			cmp r5, #4
			beq .Lend_loop_j

			/* s0 = &(c[i][j]) */
			mov r7, r5
			adds r7, r7, r4, LSL #2
			adds r7, r2, r7, LSL #2
			vldr s0, [r7]

			mov r6, #0

			.Lloop_k:
				cmp r6, #4
				beq .Lend_loop_k

				/* s1 = &(a[i][k]) */
				mov r8, r6
				adds r8, r8, r4, LSL #2
				adds r8, r0, r8, LSL #2
				vldr s1, [r8]

				/* s2 = &(b[k][j]) */
				mov r8, r5
				adds r8, r8, r6, LSL #2
				adds r8, r1, r8, LSL #2
				vldr s2, [r8]

				/* s0 += s1 * s2 */
				vmul.f32 s3, s1, s2
				vadd.f32 s0, s0, s3

				add r6, r6, #1
				b .Lloop_k

			.Lend_loop_k:
				/* c[i][j] = s0 */
				vstr s0, [r7]
				add r5, r5, #1
				b .Lloop_j

		.Lend_loop_j:
			add r4, r4, #1
			b .Lloop_i
	
	.Lend_loop_i:
		pop {r4, r5, r6, r7, r8, lr}
		bx lr

addr_mat_A: .word mat_A
addr_mat_B: .word mat_B
addr_mat_C: .word mat_C
addr_format_result: .word format_result

