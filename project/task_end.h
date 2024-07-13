/**
 * @file task_end.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __TASK_END_H__
#define __TASK_END_H__

#include "main.h"
#include "project-lcd.h"
#include "project.h"

extern TaskHandle_t TaskHandle_end;

void task_end(void *Parameters);
void task_end_init(void);

#endif