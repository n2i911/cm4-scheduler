#ifndef _OS_H
#define _OS_H

/* cm4-scheduler configurations */
#define CONFIG_MAX_TASK		6
#define CONFIG_MAX_STACKSIZE	128	/* unit in word, 128*4 = 512 bytes */
#define CONFIG_DEBUG		1
#define CONFIG_MAX_PRIORITY	5	/* 0,...,5, 0 is the highest priority */
#define CONFIG_DEFAULT_PRIORITY	2

#include <stdlib.h>
#include <string.h>

/* SoC related */
#include "stm32f4xx.h"

/* cm4-scheduler */
#include "platform.h"
#include "list.h"
#include "kernel.h"
#include "task.h"

#endif
