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

#include "task_IMU.h"

/*****************************************************************************/
/*  FreeRTOS Handles                                                         */
/*****************************************************************************/
TaskHandle_t TaskHandle_IMU;

/* QueueHandle_t for the IMU Position queue */
QueueHandle_t IMU_Queue;

/**
 * @brief
 * This task get the y values bsaed on the IMU.  When SW2 Sends a task notification, 
 * the task will send a queue with the y coordinates of IMU.
 *  @param pvParameters
 */
void task_IMU(void *Parameters)
{
    /* Wait for the SW2 to send a task notification */
    ulTaskNotifyTake(true, portMAX_DELAY);
    printf("In IMU\n\r");
    int16_t y_value;
    while(1)
    {   
        /* Get y coordinate values based on the IMU position*/
        y_value = project_IMU();

        /* Send y coordinates to the LCD via Queue*/
        xQueueSend(IMU_Queue, &y_value, portMAX_DELAY);
    }

}

void task_IMU_init(void)
{
    /* Initalize Peripherals */
    spi_init();

    /* Initialize the IMU queue that will be used to send the y coordinates to LCD */
    IMU_Queue = xQueueCreate(1,sizeof(uint16_t));

    /* Create the IMU Task */
    xTaskCreate(
        task_IMU,
        "IMU Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_IMU);
}