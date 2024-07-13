/**
 * @file task_eeprom.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __TASK_EEPROM_H__
#define __TASK_EEPROM_H__

#include "main.h"
#include "project.h"

extern TaskHandle_t TaskHandle_EEPROM;

void task_eeprom(void *Parameters);
void task_eeprom_init(void);

#endif