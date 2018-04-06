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

	/*
	 * Create idle task, which will stay in system forever.
	 * Don't kill it.
	 */
	val = task_create(handler_idle, NULL,
		(uint32_t *)&task_stack[tcb_table.count], sizeof(task_stack[tcb_table.count]), CONFIG_MAX_PRIORITY);
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

void _schedule(void)
{
	uint32_t priority = 0;
	s_tcb *tcb = NULL;

#if CONFIG_DEBUG
	uint32_t cpid __attribute__((unused));
	uint32_t npid __attribute__((unused));

	cpid = curr_task->pid;
#endif

	/* select the highest priority task */
	priority = task_highest_priority();

	/* change status of current task to ready and add to ready queue */
	curr_task->status = TASK_STATUS_READY;
	list_add_tail(&ready_queue[curr_task->priority], &curr_task->list_node);

	/* select the next task, set as current task and remove from ready queue */
	tcb = list_first_entry(&ready_queue[priority], s_tcb, list_node);
	list_del(&tcb->list_node);
	curr_task = tcb;

#if CONFIG_DEBUG
	npid = curr_task->pid;
#endif
}

void SysTick_Handler(void)
{
	uint32_t priority = 0;

#if CONFIG_DEBUG
	uint32_t cpid __attribute__((unused));
#endif

	/* select the highest priority task */
	priority = task_highest_priority();

#if CONFIG_DEBUG
	cpid = curr_task->pid;
#endif

	/*
	 * If there is other task which priority is higher or equal to current task's,
	 * doing context switch.
	 */
	if (priority <= curr_task->priority)
	{
		/* Trigger PendSV which performs the actual context switch */
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	}
}
