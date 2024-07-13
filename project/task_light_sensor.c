/**
 * @file task_light_sensor.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "task_light_sensor.h"

/*****************************************************************************/
/*  FreeRTOS Handles                                                         */
/*****************************************************************************/
TaskHandle_t TaskHandle_light_sensor;

/**
 * @brief
 * This task change the llama color based on the light received. 
 *  @param pvParameters
 */
void task_light_sensor(void *Parameters)
{
    int light;
    int reg_light = LTR_get_data(); // Get the default light value
    int color_counter = 0;  // Set counter to set which color is chosen
    int change_color = 1;
    while (1)
    {
        /* Wait for game task notification */
        ulTaskNotifyTake(true, portMAX_DELAY);
        /* Get the light value */
        light = LTR_get_data();

        /* Calculate threshold based on current and default light values */
        bool threshold = (double)light / (double)reg_light < 0.5;
        if (threshold && change_color)
        {
            /* Rotate color by incrementing color_counter */
            color_counter = (color_counter + 1) % 7;
            change_color = 0;
        }
        else if (!threshold)
        {
            change_color = 1;
        }

        /* Draw starting llama with new color */
        lcd_start_llama(chosen_color);

        /* Set color to the new color */
        switch (color_counter)
        {
        case 0:
        {
            chosen_color = LCD_COLOR_WHITE;
            break;
        }
        case 1:
        {
            chosen_color = LCD_COLOR_RED;
            break;
        }
        case 2:
        {
            chosen_color = LCD_COLOR_ORANGE;
            break;
        }
        case 3:
        {
            chosen_color = LCD_COLOR_YELLOW;
            break;
        }
        case 4:
        {
            chosen_color = LCD_COLOR_GREEN;
            break;
        }
        case 5:
        {
            chosen_color = LCD_COLOR_BLUE;
            break;
        }
        case 6:
        {
            chosen_color = LCD_COLOR_MAGENTA;
            break;
        }
        }

        cyhal_system_delay_ms(100);
    }
}

void task_light_sensor_init(void)
{
    i2c_init();

    xTaskCreate(
        task_light_sensor,
        "Light Sensor Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_light_sensor);
}