/**
 * @file task_io_expander.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "task_io_expander.h"

/*****************************************************************************/
/*  FreeRTOS Handles                                                         */
/*****************************************************************************/
TaskHandle_t TaskHandle_IO_BUFFER;

/* QueueHandle_t for the IO_Expander LEDs Queue */
QueueHandle_t IO_EXPANDER_Queue;

/**
 * @brief
 * This task turns on LED lights.  When SW2 sends a task notification, 
 * the task will read increment the active leds and send a speed value to the queue.
 *  @param pvParameters
 */
void task_io_buffer(void *Parameters)
{
    ulTaskNotifyTake(true, portMAX_DELAY);

    uint8_t active_led = 0x40;
    uint16_t io_speed = 4;
    uint8_t next_active_led = 0x40;
    uint8_t io_sw = 0xFF;

    while(1)
    {
        /* Need to read the IO Expander due to the way Interrupts from the IO Expander work */
        io_sw = io_expander_get_input_port();

        /* Set the IO Expander Outputs to the active LED */ 
	    io_expander_set_output_port(active_led); 	

        cyhal_system_delay_ms(200);

        int cur = io_speed;

        /* Rotate the active LED */
        if(active_led == 0x7F)
        {
            /* Reset to the bottom LED and default speed */
            active_led = 0x40;
            next_active_led = 0x40;
            io_speed = 4;
        }
        else
        {
            /* Continually add the active led to light up the pervious and the next led */
            next_active_led = (next_active_led >> 1);
            active_led += next_active_led;
            /* Incremenet the spped as more lights are lit up */
            io_speed += 1;
        }

        int i;

        /* Start io Expander based on the SW1 Queue */
        BaseType_t sw1 = xQueueReceive(SW1_Queue,&i, 1);
        /* Check if SW1 was pressed */
        if(sw1)
        {
            /* Send the speed of the spit to LCD */
            xQueueSend(IO_EXPANDER_Queue,&cur, portMAX_DELAY);
            /* Clear all LED lights */
            io_expander_set_output_port(0x00);
            /* Set default LEDs and Speed */
            io_speed = 4;
            active_led = 0x40;
            next_active_led = 0x40;
            
            /* Recieve the SW1 Sempaphore */
            xSemaphoreTake(Sem_SW1, portMAX_DELAY); 
        }
        
    }
}
 
void task_io_expander_init(void)
{
    /* Initialize the IO expander queue that will be used to send the speed of the spit to LCD */
    IO_EXPANDER_Queue = xQueueCreate(1,sizeof(uint16_t));

    /* Initalize IO expander LEDS */
    io_expander_set_configuration(0x80); 	// Set bit 7 as input, bits 6-0 as outputs
    io_expander_set_output_port(0x00); 		// Turn OFF all LEDs

    /* Create the IO expander Task */
    xTaskCreate(
        task_io_buffer,
        "IO BUFFER Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_IO_BUFFER);
   
}