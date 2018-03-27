#ifndef _OS_H
#define _OS_H

/* cm4-scheduler configurations */
#define CONFIG_MAX_TASK		4
#define CONFIG_MAX_TASKSIZE	64	/* 64*4 = 256 bytes */

#include <stdlib.h>
#include <string.h>

/* SoC related */
#include "stm32f4xx.h"

/* cm4-scheduler */
#include "platform.h"
#include "kernel.h"
#include "task.h"

#endif
