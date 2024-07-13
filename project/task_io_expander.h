/**
 * @file task_buttons.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __TASK_IO_EXPANDER_H__
#define __TASK_IO_EXPANDER_H__

#include "main.h"
#include "project.h"

extern TaskHandle_t TaskHandle_IO_BUFFER;
extern QueueHandle_t IO_EXPANDER_Queue;

void task_io_buffer(void *Parameters);
void task_io_expander_init(void);

#endif