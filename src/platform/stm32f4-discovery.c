#include "stm32f4xx.h"

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"

#include "os.h"

void platform_led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO D clock. */
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/* Configure the LED pin
	 * LD3 (orange): PD13
	 * LD4 (green): PD12
	 * LD5 (red): PD14
	 * LD6 (blue): PD15
	 * */
	GPIO_InitStructure.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Toggle on all leds */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
}

void platform_serial_init(void)
{
	/* TODO */
}

void __platform_init(void)
{
	/* platform init */
	platform_led_init();

	platform_serial_init();
}

void handler_toggle_led(void *p_param)
{
	uint32_t pin = (uint32_t)p_param;

	while (1)
	{
		/* Toggle off all leds
		 * LD3 (orange): PD13
		 * LD4 (green): PD12
		 * LD5 (red): PD14
		 * LD6 (blue): PD15
		 * */

		/* Toggle off all leds */
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

		/* Toggle on the led */
		HAL_GPIO_WritePin(GPIOD, pin, GPIO_PIN_SET);
	}
}

int32_t __platform_create_tasks(void)
{
	int32_t val = -1;

	val = task_create(handler_toggle_led, (void *)GPIO_PIN_12,
		(uint32_t *)&task_stack[tcb_table.count], sizeof(task_stack[tcb_table.count]));

	if (val == -1)
		dead();

	val = task_create(handler_toggle_led, (void *)GPIO_PIN_13,
		(uint32_t *)&task_stack[tcb_table.count], sizeof(task_stack[tcb_table.count]));

	if (val == -1)
		dead();

	val = task_create(handler_toggle_led, (void *)GPIO_PIN_14,
		(uint32_t *)&task_stack[tcb_table.count], sizeof(task_stack[tcb_table.count]));

	if (val == -1)
		dead();

	val = task_create(handler_toggle_led, (void *)GPIO_PIN_15,
		(uint32_t *)&task_stack[tcb_table.count], sizeof(task_stack[tcb_table.count]));

	if (val == -1)
		dead();

	return val;
}
