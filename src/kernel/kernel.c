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

void main(void)
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

	if (curr_task->status == TASK_STATUS_WAITING)
	{
		list_add_tail(&waiting_queue[curr_task->reason], &curr_task->list_node);
	}
	else
	{
		/* change status of current task to ready and add to ready queue */
		curr_task->status = TASK_STATUS_READY;
		list_add_tail(&ready_queue[curr_task->priority], &curr_task->list_node);
	}

	/* select the highest priority task */
	priority = task_highest_priority();

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
	s_tcb *tcb = NULL;
	s_tcb *n = NULL;
	uint32_t priority = 0;

#if CONFIG_DEBUG
	uint32_t cpid __attribute__((unused));
	uint32_t pri __attribute__((unused));
	uint32_t pid __attribute__((unused));
#endif

	/*
	 * Check the waiting queue.
	 * If time's up, move to ready queue from waiting queue
	 */
	if (!list_empty(&waiting_queue[TASK_WAIT_TIME]))
	{
		list_for_each_entry(tcb, &waiting_queue[TASK_WAIT_TIME], list_node)
		{
			tcb->timeup--;

			if (tcb->timeup == 0)
			{
				n = tcb;
				tcb = list_entry(n->list_node.next, s_tcb ,list_node);
#if CONFIG_DEBUG
				pid = n->pid;
				pri = n->priority;

#endif
				/*
				 * Remove from waiting queue and add to ready queue
				 */
				list_del(&n->list_node);
				n->status = TASK_STATUS_READY;
				list_add_tail(&ready_queue[n->priority], &n->list_node);
			}
		}
	}

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

void c_SVC_Handler(uint32_t *param)
{
	s_tcb *task = NULL;
	s_tcb *p = NULL;
	s_tcb *n = NULL;
	uint32_t svc_number = 0;

	uint32_t stack_r0 = param[0];
	uint32_t stack_r1 __attribute__((unused)) = param[1];
	uint32_t stack_r2 __attribute__((unused)) = param[2];
	uint32_t stack_r3 __attribute__((unused)) = param[3];
	uint32_t stack_12 __attribute__((unused)) = param[4];
	uint32_t stack_lr __attribute__((unused)) = param[5];
	uint32_t stack_pc = param[6];
	uint32_t stack_psr __attribute__((unused)) = param[7];

	svc_number = ((char*)stack_pc)[-2];

	switch (svc_number)
	{
	case 0:
		/* sleep() */
		curr_task->timeup = stack_r0 * CONFIG_SYSTICK;
		curr_task->status = TASK_STATUS_WAITING;
		curr_task->reason = TASK_WAIT_TIME;

		/* Trigger PendSV which performs the actual context switch */
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
		break;

	case 1:
		/*
		 * task_suspend(), can't suspend the idle task
		 *
		 * TODO: Using stack frame, check if user would suspend the idle task
		 */
		task = (s_tcb *) stack_r3;

		task->status = TASK_STATUS_WAITING;
		task->reason = TASK_WAIT_EVENT;

		/* suspend current task itself */
		if (task == curr_task)
		{
			/* Trigger PendSV which performs the actual context switch */
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
		}
		else
		{
			list_del(&task->list_node);
			list_add_tail(&waiting_queue[task->reason], &task->list_node);
		}

		break;
	case 2:

		/* task_resume() */
		task = (s_tcb *) stack_r0;

		/*
		 * Search the waiting queue
		 */
		if (!list_empty(&waiting_queue[TASK_WAIT_EVENT]))
		{
			list_for_each_entry(p, &waiting_queue[TASK_WAIT_EVENT], list_node)
			{
				if (p == task)
				{
					n = p;
					p = list_entry(n->list_node.next, s_tcb, list_node);

					/*
					 * Remove from waiting queue and add to ready queue
					 */
					list_del(&n->list_node);
					n->status = TASK_STATUS_READY;
					list_add_tail(&ready_queue[n->priority], &n->list_node);
				}
			}
		}

		break;

	case 3:
		/* task_resumeAll() */

		/*
		 * Search the waiting queue, resume all suspend task
		 */
		if (!list_empty(&waiting_queue[TASK_WAIT_EVENT]))
		{
			list_for_each_entry(p, &waiting_queue[TASK_WAIT_EVENT], list_node)
			{
				n = p;
				p = list_entry(n->list_node.next, s_tcb, list_node);

				/*
				 * Remove from waiting queue and add to ready queue
				 */
				list_del(&n->list_node);
				n->status = TASK_STATUS_READY;
				list_add_tail(&ready_queue[n->priority], &n->list_node);
			}
		}

	default:
		break;
	}
}
