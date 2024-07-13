/**
 * @file task_LCD.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __TASK_LCD_H__
#define __TASK_LCD_H__

#include "main.h"
#include "project-images.h"
#include "project.h"

extern TaskHandle_t TaskHandle_LCD;
extern TaskHandle_t TaskHandle_SPIT;
extern TaskHandle_t TaskHandle_RECEIVE;
extern QueueHandle_t Win_Queue;
extern QueueHandle_t UART_QUEUE;
extern QueueHandle_t END_QUEUE;


struct proj{
    int y_pos;
    int x_pos;
    int speed;
};

void task_draw_receive(void *Parameters);
void task_draw_llama(void *Parameters);
void task_draw_spit(void *Parameters);
void task_LCD_init(void);

#endif