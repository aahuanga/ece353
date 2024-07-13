/**
 * @file task_buttons.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "task_LCD.h"

#define CURSOR_SIZE 5

/*****************************************************************************/
/*  FreeRTOS Handles                                                         */
/*****************************************************************************/
TaskHandle_t TaskHandle_LCD;
TaskHandle_t TaskHandle_SPIT;

/* QueueHandle_t for the Player Win Queue */
TaskHandle_t TaskHandle_RECEIVE;
QueueHandle_t Win_Queue;
QueueHandle_t END_QUEUE;

/* QueueHandle_t for the UART Queue */
QueueHandle_t UART_QUEUE;

/* Initalize the Projectile Queue (includes the spit's x and y coordinates and speed)*/
struct Projectile_Queue *spits;

void task_draw_receive(void *Parameters)
{
    while (1)
    {
        int speed = 0;
        int y = 0 ;
        xQueueReceive(UART_send, &speed, portMAX_DELAY);
        xQueueReceive(UART_send, &y, portMAX_DELAY);
        printf("Drawing: Speed %i Y %i\n\r", speed, y);
        speed = 5;


        if (player)
        {
            int x = 320;

            while (1)
            {
                vTaskDelay(20);
                if ((x) > 48)
                {
                    lcd_spit(x, y, 0);
                    x -= speed;
                }
                else
                {
                    if ( (y_coord - 20) < y && (y_coord +20 ) > y)
                    {
                        printf("Win in draw \n\r");
                        game = GAME_END;
                        int send = player ? 0 : 1;
                        xQueueSend(Win_Queue, &send, portMAX_DELAY);
                        xQueueSend(END_QUEUE, &send, portMAX_DELAY);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {

            int x = 0;

            while (1)
            {
                vTaskDelay(20);
                if ((x) < 280)
                {
                    lcd_spit(x, y, 1);
                    x += speed;
                }
                else
                {
                    if( (y_coord - 20) < y && (y_coord +20 ) > y)
                    {
                        game = GAME_END;
                        int send = player ? 0 : 1;
                        xQueueSend(Win_Queue, &send, portMAX_DELAY);
                        xQueueSend(END_QUEUE, &send, portMAX_DELAY);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}
void task_draw_llama(void *Parameters)
{
    BaseType_t status;
    int16_t pos;

    while (1)
    {
        /* Wait to recieve the IMU Position */
        status = xQueueReceive(IMU_Queue, &pos, 5);

        if (status)
        {
            // printf("%i\n\r", pos / 100);
            pos = pos / 1000;
            if (pos > 80)
                pos = 80;
            else if (pos < -80)
                pos = -80;

            if ((y_coord + pos - llamaHeightPixels / 2) < 0)
            {
            }
            else if ((y_coord + pos + llamaHeightPixels / 2) > 250)
            {
            }
            else
                y_coord += pos;

            if (player)
            {
                lcd_draw_image(
                    x_coord, // 48
                    y_coord, // 200
                    llamaWidthPixels,
                    llamaHeightPixels,
                    llamaReverseBitmaps,
                    chosen_color,
                    LCD_COLOR_BLACK);
            }
            else
            {
                lcd_draw_image(
                    x_coord, // x_coord, // 280
                    y_coord, // y_coord, // 200
                    llamaWidthPixels,
                    llamaHeightPixels,
                    llamaBitmaps,
                    chosen_color,
                    LCD_COLOR_BLACK);
            }
        }
        if(game == GAME_END)
        {
            while(1)
            {

            }
        }
        // count = (count + 1) % 10;
    }
}

void task_draw_spit(void *Parameters)
{
    // Will need to use task notification to receive coordinates of the llama based on the IMU
    ulTaskNotifyTake(true, portMAX_DELAY);

    while (1)
    {

        uint8_t speed = 0;
        BaseType_t io_status = xQueueReceive(IO_EXPANDER_Queue, &speed, portMAX_DELAY);

        if (player)
        {
            int x = x_coord + 40;
            uint8_t y = y_coord - 20;

            if (io_status)
            {
                while (1)
                {
                    vTaskDelay(20);
                    if ((x) < 310)
                    {
                        lcd_spit(x, y, player);
                        x += speed;
                    }
                    else
                    {
                        xQueueSend(UART_QUEUE, &speed, portMAX_DELAY);
                        xQueueSend(UART_QUEUE, &y, portMAX_DELAY);
                        break;
                    }
                }
            }
        }
        else
        {

            int x = x_coord - 40;
            uint8_t y = y_coord - 20;

            if (io_status)
            {
                while (1)
                {
                    vTaskDelay(20);
                    if ((x) > 10)
                    {
                        lcd_spit(x, y, player);
                        x -= speed;
                    }
                    else
                    {
                        xQueueSend(UART_QUEUE, &speed, portMAX_DELAY);
                        xQueueSend(UART_QUEUE, &y, portMAX_DELAY);

                        break;
                    }
                }
            }
        }
        xSemaphoreGive(Sem_SW1);
       
    }
}

void task_LCD_init(void)
{
    Win_Queue = xQueueCreate(1, sizeof(int));

    UART_QUEUE = xQueueCreate(2, sizeof(uint8_t));

    END_QUEUE = xQueueCreate(1, sizeof(int));
    spits = p_circular_buffer_init(50);
    xTaskCreate(
        task_draw_spit,
        "Draw spit Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_SPIT);
    xTaskCreate(
        task_draw_receive,
        "LCD Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_RECEIVE);
    xTaskCreate(
        task_draw_llama,
        "LCD Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_LCD);
}
