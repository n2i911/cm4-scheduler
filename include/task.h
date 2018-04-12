#ifndef _TASK_H
#define _TASK_H

typedef struct
{
	uint32_t stack[CONFIG_MAX_STACKSIZE-16];
	uint32_t r11;
	uint32_t r10;
	uint32_t r9;
	uint32_t r8;
	uint32_t r7;
	uint32_t r6;
	uint32_t r5;
	uint32_t r4;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;

} s_stack;

enum e_task_status
{
	TASK_STATUS_RUNNING = 0,
	TASK_STATUS_READY,
	TASK_STATUS_WAITING
};

enum e_task_wait_reason
{
	TASK_WAIT_TIME = 0,
	TASK_WAIT_EVENT,
	TASK_WAIT_MAX

};

typedef struct
{
	volatile uint32_t *sp;
	void (*handler) (void *p_param);
	volatile void *p_param;
	enum e_task_status status;
	volatile uint32_t pid;
	volatile uint32_t priority;
	volatile uint32_t timeup;
	enum e_task_wait_reason reason;
	struct s_list_node list_node;
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
extern s_stack task_stack[];
extern struct s_list_node ready_queue[];
extern struct s_list_node waiting_queue[];

int32_t task_init(void);

int32_t task_create(void (*handler) (void *p_param), void *p_param,
	uint32_t *sp_stack, size_t stack_size, uint32_t priority);

uint32_t task_highest_priority(void);
int32_t task_start(void);
void task_finish(void);
#endif
