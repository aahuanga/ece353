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
#ifndef __TASK_LIGHT_SENSOR_H__
#define __TASK_LIGHT_SENSOR_H__

#include "main.h"
#include "project-lcd.h"
#include "project.h"

extern TaskHandle_t TaskHandle_light_sensor;
extern SemaphoreHandle_t Sem_Light;

void task_light_sensor(void *Parameters);
void task_light_sensor_init(void);

#endif