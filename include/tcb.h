#ifndef _TCB_H
#define _TCB_H

enum e_task_status
{
	TASK_STATUS_IDLE = 0,
	TASK_STATUS_ACTIVE
};

typedef struct
{
	volatile uint32_t *sp;
	void (*handler) (void *p_param);
	volatile void *p_param;
	enum e_task_status status;
	volatile uint32_t pid;
} s_tcb;

typedef struct
{
	s_tcb tcb_table[CONFIG_MAX_TASKS];

} s_tcb_table;

int task_create(void (*handler) (void *p_param), void *p_param,
		uint32_t *sp_stack, size_t stack_size);

#endif
