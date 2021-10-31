/*
 * threads.c
 *
 *  Created on: Oct 30, 2021
 *      Author: muaddib
 */


#include "threads.h"
#include "display.h"
#include "cmsis_os.h"

static osThreadId_t _display_task_handle;

const osThreadAttr_t display_task_attributes =
{.name = "defaultTask", .priority = (osPriority_t) osPriorityNormal,
        .stack_size = 128 * 4};


void threads_start(void)
{
    _display_task_handle = osThreadNew(display_task, (void *)display_task_attributes.name,
            &display_task_attributes);
}
