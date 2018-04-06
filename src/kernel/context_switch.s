
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global PendSV_Handler
.type PendSV_Handler, %function
PendSV_Handler:

	/* Disable interrupts */
	cpsid i

	/* Save register to current task's stack */
	mrs r0, psp
	stmdb r0!, {r4-r11}

	/* Save psp to current task's tcb */
	ldr r1, =curr_task
	ldr r2, [r1]
	str r0, [r2]

	/* select the next task */
	push {lr}
	bl _schedule
	pop {lr}

	/* Get sp from next task's stack */
	ldr r1, =curr_task
	ldr r2, [r1]
	ldr r0, [r2]

	/* restore register from next task's stack */
	ldmia r0!, {r4-r11}

	msr psp, r0

	/* Enable interrupts */
	cpsie i

	bx lr

.size PendSV_Handler, .-PendSV_Handler
