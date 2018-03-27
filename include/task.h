#ifndef _TASK_H
#define _TASK_H

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
	s_tcb tcb[CONFIG_MAX_TASK];
	uint32_t current_task;
	size_t count;
} s_tcb_table;

enum e_os_status
{
	OS_STATUS_DEFAULT = 0,
	OS_STATUS_START
};

extern enum e_os_status os_status;

extern s_tcb_table tcb_table;
extern s_tcb *curr_task;
extern s_tcb *next_task;
extern uint32_t task_stack[][CONFIG_MAX_TASKSIZE];

int32_t task_init(void);

int32_t task_create(void (*handler) (void *p_param), void *p_param,
		uint32_t *sp_stack, size_t stack_size);

int32_t task_start(void);

void task_finish(void);
#endif
