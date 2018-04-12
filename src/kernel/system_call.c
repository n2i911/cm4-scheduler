#include "os.h"

unsigned sleep(unsigned seconds)
{
	__asm("svc #0");

	return 0;
}

void task_suspend(s_tcb *task)
{
	/* If task is NULL, suspend the current task itself */
	if (task == NULL)
	{
		task = curr_task;
	}

	__asm("svc #1");
}

void task_resume(s_tcb *task)
{
	__asm("svc #2");
}

void task_resumeAll(void)
{
	__asm("svc #3");
}

void delay(uint32_t count)
{
	while(count--);
}
