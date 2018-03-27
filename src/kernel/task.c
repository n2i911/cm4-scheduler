#include "os.h"

s_tcb_table tcb_table;
s_tcb *curr_task;
s_tcb *next_task;
uint32_t task_stack[CONFIG_MAX_TASK][CONFIG_MAX_TASKSIZE];

int32_t task_init(void)
{
	void *addr;
	int32_t val = -1;

	addr = memset(&tcb_table, 0, sizeof(tcb_table));

	if ((s_tcb_table *)addr == &tcb_table)
	{
		val = 0;
	}

	curr_task = &tcb_table.tcb[tcb_table.current_task];

	return val;
}

int32_t task_create(void (*handler) (void *p_param), void *p_param,
		uint32_t *sp_stack, size_t stack_size)
{
	int32_t val = -1;
	size_t count = tcb_table.count;

	if (count >= CONFIG_MAX_TASK)
	{
		val = -1;
	}
	else if (count < CONFIG_MAX_TASK)
	{
		uint32_t offset = (stack_size)/sizeof(uint32_t);

		/* Set the sp to the top of stack first and subtract 16 words
		 * which would used to save register:
		 * xPSR, PC, LR, R12, R3, R2, R1, R0, R4, R5, R6, R7, R8, R9,
		 * R10, R11
		 *
		 * R13 has been saved in tcb
		 */
		tcb_table.tcb[tcb_table.count].sp = sp_stack + offset - 16;
		tcb_table.tcb[tcb_table.count].handler = handler;
		tcb_table.tcb[tcb_table.count].p_param= p_param;
		tcb_table.tcb[tcb_table.count].status = TASK_STATUS_IDLE;
		tcb_table.tcb[tcb_table.count].pid = count;

		/* initial stack frame for task */
		sp_stack[offset - 1] = 0x01000000; // xPSR
		sp_stack[offset - 2] = (uint32_t) handler; // PC
		sp_stack[offset - 3] = (uint32_t) task_finish; // LR
		sp_stack[offset - 4] = (count+1)*1024 + 12; // R12
		sp_stack[offset - 5] = (count+1)*1024 + 3; // R3
		sp_stack[offset - 6] = (count+1)*1024 + 2; // R2
		sp_stack[offset - 7] = (count+1)*1024 + 1; // R1
		sp_stack[offset - 8] = (uint32_t) p_param; // R0
		sp_stack[offset - 9] = (count+1)*1024 + 4; // R4
		sp_stack[offset - 10] = (count+1)*1024 + 5; // R5
		sp_stack[offset - 11] = (count+1)*1024 + 6; // R6
		sp_stack[offset - 12] = (count+1)*1024 + 7; // R7
		sp_stack[offset - 13] = (count+1)*1024 + 8; // R8
		sp_stack[offset - 14] = (count+1)*1024 + 9; // R9
		sp_stack[offset - 15] = (count+1)*1024 + 10; // R10
		sp_stack[offset - 16] = (count+1)*1024 + 11; // R11

		tcb_table.count++;

		val = 0;
	}

	return val;
}

int32_t task_start(void)
{
	int32_t val = -1;

	/* Lower the priority for PendSV and higher the priority
	 * for SysTick
	 */
	NVIC_SetPriority(PendSV_IRQn, 0xff);
	NVIC_SetPriority(SysTick_IRQn, 0x00);

	/* Start SysTick */
	SysTick_Config(SystemCoreClock);

	/* Set PSP to top of task */
	__set_PSP((uint32_t)curr_task->sp + (16*4));

	/* Switch to use Process Stack, unprivileged state */
	__set_CONTROL(0x3);

	/* Execute ISB after changing CONTROL */
	__ISB();

	curr_task->handler((void *)curr_task->p_param);

	return val;
}

void task_finish(void)
{
	while (1);
}
