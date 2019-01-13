.syntax unified
.thumb_func
.globl blink
.globl TIM2_IRQHandler
/*
TIM2_IRQHandler:
	push {r7, lr}
	// save GPIOA->BSRR address to r2 and r3
	movs r3, #144
	lsls r3, r3, #23
	mov r2, r3
	// process
	ldr r2, [r2, #24]
	movs r1, #32
	orrs r2, r1
	str r2, [r3, #24]
	// return
	pop {r7, pc}
*/
TIM2_IRQHandler:
	push {r7, lr}
	bl blink
	// save TIM2->SR address to r2 and r3
	movs r3, #64
	lsls r3, r3, #24
	mov r2, r3
	// process
	ldr r2, [r2, #16]
	movs r1, #1
	bics r2, r2, r1
	str r2, [r3, #16]
	// return
	pop {r7, pc}

