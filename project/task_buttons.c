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

#include "task_buttons.h"

#define CURSOR_SIZE 5

/* TaskHandle_T variables for the SW1 and SW2 */
TaskHandle_t TaskHandle_SW1;
TaskHandle_t TaskHandle_SW2;

/* QueueHandle_t for the SW1 Pressed queue */
QueueHandle_t SW1_Queue;

/* Allocate a semaphore that will be used to control the lcd by drawing the llama's spit*/
SemaphoreHandle_t Sem_SW1;

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
cyhal_gpio_callback_data_t sw1_callback_data;
cyhal_gpio_callback_data_t sw2_callback_data;

/*****************************************************************************/
/* Interrupt Handlers                                                        */
/*****************************************************************************/
void sw1_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
     it will get set to pdTRUE inside the interrupt safe API function if a
     context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Send a notification directly to the task to which interrupt processing
     * is being deferred.
     */
    vTaskNotifyGiveFromISR(TaskHandle_SW1, &xHigherPriorityTaskWoken);

    /* ADD CODE */
    /* Call the function that will force the task that was running prior to the
     * interrupt to yield*/
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void sw2_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
     it will get set to pdTRUE inside the interrupt safe API function if a
     context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Send a notification directly to the task to which interrupt processing
     * is being deferred.
     */
    vTaskNotifyGiveFromISR(TaskHandle_SW2, &xHigherPriorityTaskWoken);

    /* ADD CODE */
    /* Call the function that will force the task that was running prior to the
     * interrupt to yield*/
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*****************************************************************************/
/* Peripheral Initialization                                                 */
/*****************************************************************************/
void sw1_irq_enable(void)
{
    sw1_callback_data.callback = sw1_handler;
    cyhal_gpio_register_callback(PIN_SW1, &sw1_callback_data);
    cyhal_gpio_enable_event(PIN_SW1, CYHAL_GPIO_IRQ_FALL, 3, true);
}
void sw2_irq_enable(void)
{
    sw2_callback_data.callback = sw2_handler;
    cyhal_gpio_register_callback(PIN_SW2, &sw2_callback_data);
    cyhal_gpio_enable_event(PIN_SW2, CYHAL_GPIO_IRQ_FALL, 3, true);
}

/**
 * @brief 
 * This task is a bottom half task for the IO pin that generates an interrupt when the user
 * pressed SW1.  This task will wait indefinitely for a task notification from the 
 * IO pins handler.  Once the task notification is received, the task will send a queue to lcd.
 * @param pvParameters 
 */
void task_sw1(void *Parameters)
{
    while (1)
    {
        /* Wait for the SW1 Interrupt Handler to send a task notification */
        ulTaskNotifyTake(true, portMAX_DELAY);
        if(game == GAME_PLAY)
        {
            printf("SW1 Pressed \n\r");
            int i = 1;
            xQueueSend(SW1_Queue, &i, 5);

        }
      
    }
}

/**
 * @brief 
 * This task is a bottom half task for the IO pin that generates an interrupt when the user
 * presses SW2.  This task will wait indefinitely for a task notification from the 
 * IO pins handler.  Once the task notification is received, the task will start the game and send task
 * notifications to IMU, IO Expander, LCD, and UART.
 * @param pvParameters 
 */
void task_sw2(void *Parameters)
{
    while (1)
    {
        /* Wait for the SW2 Interrupt Handler to send a task notification */
        ulTaskNotifyTake(true, portMAX_DELAY);

        printf("sw2\n\r");
        if (game == GAME_START)
        {

            lcd_clear_screen(LCD_COLOR_BLACK);

            printf("In SW2\n\r");

            /* Send task notifications to IMU, IO Expander, LCD (draw spit), and UART */
            xTaskNotifyGive(TaskHandle_IMU);
            xTaskNotifyGive(TaskHandle_IO_BUFFER);
            xTaskNotifyGive(TaskHandle_SPIT);
            xTaskNotifyGive(TaskHandle_UART_tx);

            /* Set Game to Game Play state*/
            game = GAME_PLAY;
        }
    }
}

void task_buttons_init(void)
{
    /* Initialize and enable peripherals */
    //push_buttons_init();
    sw1_irq_enable();
    sw2_irq_enable();

    /* Initalize Sempaphore for SW1*/
    Sem_SW1 = xSemaphoreCreateBinary();

    /* Initialize the SW1 queue that will be used to send the speed of spit
     * based on the IO expander
     */
    SW1_Queue = xQueueCreate(1,sizeof(uint16_t));

    /* Create the SW1 and SW2 Task */
    xTaskCreate(
        task_sw1,
        "SW1 Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_SW1);

    xTaskCreate(
        task_sw2,
        "SW2 Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_SW2);
}