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
#ifndef __TASK_BUTTONS_H__
#define __TASK_BUTTONS_H__

#include "main.h"
#include "project.h"

extern TaskHandle_t TaskHandle_SW1;
extern TaskHandle_t TaskHandle_SW2;
extern QueueHandle_t SW1_Queue;
extern SemaphoreHandle_t Sem_SW1;

void task_sw1(void *Parameters);
void task_sw2(void *Parameters);
void task_buttons_init(void);

#endif