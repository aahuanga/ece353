/**
 * @file task_uart.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "task_UART.h"
#include "project.h"
#include "project-lcd.h"

TaskHandle_t TaskHandle_UART_rx;
TaskHandle_t TaskHandle_UART_tx;
QueueHandle_t UART_send;
int disable = 0;
// cyhal_uart_t project_remote_uart_obj;

void task_UART_tx(void *Parameters)
{
    while (1)
    {
        //printf("uart loop\n\r");
        if (game == GAME_START)
        {
            printf("about to send\n\r");
            ulTaskNotifyTake(true, portMAX_DELAY);
            if (disable)
            {
                continue;
            }
            uint8_t message[1];
            message[0] = 244;
            remote_uart_tx_string_polling(message);
            remote_uart_tx_string_polling("\n");
            printf("sent\n\r");
        }
        else if (game == GAME_PLAY)
        {
            uint8_t speed[1];
            uint8_t y[1];

            BaseType_t in = xQueueReceive(UART_QUEUE, &speed, 5);
            if(in)
            {
                xQueueReceive(UART_QUEUE, &y, portMAX_DELAY);
                printf("Send: Speed %i, Y %i\n\r", speed[0], y[0]);
                remote_uart_tx_string_polling(y);
                remote_uart_tx_string_polling(speed);
                remote_uart_tx_string_polling("\n");
            }
        }
        else if (game == GAME_END)
        {
            int test = 0;
            xQueueReceive(END_QUEUE, &test, portMAX_DELAY);
            uint8_t end[1];
            end[0] = 243;
            printf("Sending end\n\r");
            remote_uart_tx_string_polling(end);
            remote_uart_tx_string_polling("\n");
        }
    }
}

void task_UART_rx(void *Parameters)
{
    while (1)
    {
        uint8_t message[2];
        if (remote_uart_rx_string_polling(message))
        {
            printf("%i\n\r", message[0]);
            printf("%i\n\r", message[1]);

            if ((message[0] == 244))
            {
                lcd_clear_screen(LCD_COLOR_BLACK);
                player = 1;
                x_coord = 48;
                xTaskNotifyGive(TaskHandle_IMU);
                xTaskNotifyGive(TaskHandle_IO_BUFFER);
                xTaskNotifyGive(TaskHandle_SPIT);
                game = GAME_PLAY;
                disable = 1;
                xTaskNotifyGive(TaskHandle_UART_tx);
            }
            else if (message[0] == 243)
            {
                int send = player;
                xQueueSend(Win_Queue, &send, portMAX_DELAY);
                printf("Received end\n\r");
            }
            else
            {
                int y = (int)message[0];
                int speed = (int)message[1];
                printf("Receive: Speed %i, Y %i\n\r", speed, y);
                xQueueSend(UART_send, &speed, portMAX_DELAY);
                xQueueSend(UART_send, &y, portMAX_DELAY);
            }
        }
    }
}

void task_UART_init(void)
{
    /* Initialize the remote UART */
    UART_send = xQueueCreate(2, sizeof(int));
    remote_uart_init();

    xTaskCreate(
        task_UART_rx,
        "UARTrx Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_UART_rx);

    xTaskCreate(
        task_UART_tx,
        "UART_tx Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_UART_tx);
}