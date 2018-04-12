#ifndef _SYSTEM_H
#define _SYSTEM_H

/* suspend execution for an interval of time */
unsigned sleep(unsigned seconds);

void task_suspend(s_tcb *task);

void task_resume(s_tcb *task);

void task_resumeAll(void);

void delay(uint32_t count);

#endif
