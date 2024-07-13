/**
 * @file task_uart.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __TASK_UART_H__
#define __TASK_UART_H__

#include "main.h"
#include "project.h"

extern TaskHandle_t TaskHandle_UART_tx;
extern TaskHandle_t TaskHandle_UART_rx;
extern QueueHandle_t UART_send;
//extern cyhal_uart_t project_remote_uart_obj;

void task_UART_rx(void *Parameters);
void task_UART_tx(void *Parameters);
void task_UART_init(void);

#endif