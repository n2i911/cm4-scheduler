#include "os.h"

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
	/* choose the next task */
	curr_task = &tcb_table.tcb[tcb_table.current_task];

	tcb_table.current_task++;
	if (tcb_table.current_task >= CONFIG_MAX_TASK)
		tcb_table.current_task = 0;

	next_task = &tcb_table.tcb[tcb_table.current_task];

	/* Trigger PendSV which performs the actual context switch */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
