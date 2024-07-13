/**
 * @file task_IMU.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __TASK_IMU_H__
#define __TASK_IMU_H__

#include "project.h"
#include "main.h"

extern TaskHandle_t TaskHandle_IMU;
extern QueueHandle_t IMU_Queue;

void task_IMU(void *Parameters);
void task_IMU_init(void);

#endif