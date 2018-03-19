#include "os.h"

void HardFault_Handler(void)
{
	while (1);
}

int main(void)
{
	/* Init peripherals of target board */
	platform_init();

	while(1);
}

void SysTick_Handler(void)
{
	/* Trigger PendSV which performs the actual context switch: */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
