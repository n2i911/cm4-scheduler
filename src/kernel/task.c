#include "os.h"

s_tcb_table tcb_table;
s_tcb *curr_task;
s_tcb *next_task;
s_stack task_stack[CONFIG_MAX_TASK];
struct s_list_node ready_queue[CONFIG_MAX_PRIORITY+1];
struct s_list_node waiting_queue[TASK_WAIT_MAX];

int32_t task_init(void)
{
	uint8_t i;
	void *addr;
	int32_t val = -1;

	addr = memset(&tcb_table, 0, sizeof(tcb_table));

	if ((s_tcb_table *)addr == &tcb_table)
	{
		val = 0;
	}

	/*
	 * initial the ready queue and waiting queue
	 */
	for (i = 0; i < TASK_WAIT_MAX; i++)
	{
		init_list_node(&waiting_queue[i]);
	}

	for (i = 0; i <= CONFIG_MAX_PRIORITY; i++)
	{
		init_list_node(&ready_queue[i]);
	}

	return val;
}

int32_t task_create(void (*handler) (void *p_param), void *p_param,
		uint32_t *sp_stack, size_t stack_size, uint32_t priority)
{
	int32_t val = -1;
	size_t count = tcb_table.count;
	s_stack *stack = NULL;

	if (count >= CONFIG_MAX_TASK)
	{
		val = -1;
	}
	else if (count < CONFIG_MAX_TASK)
	{
		if (priority > CONFIG_MAX_PRIORITY)
		{
			val = -1;
		}
		else
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
			tcb_table.tcb[tcb_table.count].status = TASK_STATUS_READY;

			/* TODO: Using different method to assign the pid */
			tcb_table.tcb[tcb_table.count].pid = count;
			tcb_table.tcb[tcb_table.count].priority = priority;

			/* add task to ready queue according to the priority */
			init_list_node(&tcb_table.tcb[tcb_table.count].list_node);
			list_add_tail(&ready_queue[priority], &tcb_table.tcb[tcb_table.count].list_node);

			/* initial stack frame for task */
#if 0

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
#else
			stack = (s_stack *)sp_stack;
			stack->psr = 0x01000000; // xPSR
			stack->pc = (uint32_t) handler; // PC
			stack->lr = (uint32_t) task_finish; // LR
			stack->r12 = (count+1)*1024 + 12; // R12
			stack->r3 = (count+1)*1024 + 3; // R3
			stack->r2 = (count+1)*1024 + 2; // R2
			stack->r1 = (count+1)*1024 + 1; // R1
			stack->r0 = (uint32_t) p_param; // R0
			stack->r4 = (count+1)*1024 + 4; // R4
			stack->r5 = (count+1)*1024 + 5; // R5
			stack->r6 = (count+1)*1024 + 6; // R6
			stack->r7 = (count+1)*1024 + 7; // R7
			stack->r8 = (count+1)*1024 + 8; // R8
			stack->r9 = (count+1)*1024 + 9; // R9
			stack->r10 = (count+1)*1024 + 10; // R10
			stack->r11 = (count+1)*1024 + 11; // R11
#endif
			tcb_table.count++;

			val = 0;
		}
	}

	return val;
}

uint32_t task_highest_priority(void)
{
	uint32_t priority = 0;

	for (priority = 0; list_empty(&ready_queue[priority]) && priority <= CONFIG_MAX_PRIORITY; priority++);

	return priority;
}

int32_t task_start(void)
{
	int32_t val = -1;
	uint32_t priority = 0;
	s_tcb *tcb = NULL;

	/* select the highest priority task */
	priority = task_highest_priority();
	tcb = list_first_entry(&ready_queue[priority], s_tcb, list_node);
	list_del(&tcb->list_node);
	curr_task = tcb;

	/* Lower the priority for PendSV and higher the priority
	 * for SysTick
	 */
	NVIC_SetPriority(PendSV_IRQn, 0xff);
	NVIC_SetPriority(SysTick_IRQn, 0x00);

	/* Start SysTick */
	SysTick_Config(SystemCoreClock/CONFIG_SYSTICK);

	/* Set PSP to top of task */
	__set_PSP((uint32_t)curr_task->sp + (16*4));

	/* Switch to use Process Stack, unprivileged state */
	__set_CONTROL(0x3);

	/* Execute ISB after changing CONTROL */
	__ISB();

	curr_task->status = TASK_STATUS_RUNNING;
	curr_task->handler((void *)curr_task->p_param);

	return val;
}

void task_finish(void)
{
	while (1);
}
