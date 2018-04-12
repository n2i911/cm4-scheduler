
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb


.global SVC_Handler
.type SVC_Handler, %function
SVC_Handler:

	push {lr}

	mrs r0, psp

	bl c_SVC_Handler

	pop {lr}
	bx lr

.size SVC_Handler, .-SVC_Handler
