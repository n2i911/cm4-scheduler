#include "os.h"

void handler_idle(void *p_param)
{
	while (1);
}

void dead(void)
{
	while (1);
}

void HardFault_Handler(void)
{
	while (1);
}

int main(void)
{
	int32_t val = -1;

	/* Init peripherals of target board */
	platform_init();

	/* Init task table */
	val = task_init();

	if (val != 0)
		dead();

	/* Create idle task */
	val = task_create(handler_idle, NULL,
		(uint32_t *)&task_stack[tcb_table.count], sizeof(task_stack[tcb_table.count]));
	if (val != 0)
		dead();

	/* Create tasks */
	val = platform_create_tasks();
	if (val != 0)
		dead();

	task_start();

	/* Actually, would not archive here */
	while(1);
}

void SysTick_Handler(void)
{
#if CONFIG_DEBUG
	uint32_t cpid __attribute__((unused));
	uint32_t npid __attribute__((unused));
#endif

	/* choose the next task */
	curr_task = &tcb_table.tcb[tcb_table.current_task];
	curr_task->status = TASK_STATUS_READY;

#if CONFIG_DEBUG
	cpid = curr_task->pid;
#endif

	tcb_table.current_task++;
	if (tcb_table.current_task >= tcb_table.count)
		tcb_table.current_task = 0;

	next_task = &tcb_table.tcb[tcb_table.current_task];
	next_task->status = TASK_STATUS_RUNNING;
#if CONFIG_DEBUG
	npid = next_task->pid;
#endif

	/* Trigger PendSV which performs the actual context switch */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
