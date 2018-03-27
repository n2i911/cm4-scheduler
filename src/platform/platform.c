#include "os.h"

void __platform_init(void);
int32_t __platform_create_tasks(void);

void platform_init(void)
{
	__platform_init();
}

int32_t platform_create_tasks(void)
{
	int32_t val = -1;

	/* Create tasks */
	val = __platform_create_tasks();

	return val;
}
