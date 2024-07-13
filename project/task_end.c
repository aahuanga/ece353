/**
 * @file task_end.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "task_end.h"

/* TaskHandle_T variables for the end screen */
TaskHandle_t TaskHandle_end;

/**
 * @brief
 * This task print out the end scores.  When EEPROM sends a task notificatoin,
 * the task will read EEPROM and display score.
 *  @param pvParameters
 */
void task_end(void *Parameters)
{
    uint16_t eeprom_address = 0x0000;
    while (1)
    {
        /* Wait for the LCD (end of game) to send a task notification */
        ulTaskNotifyTake(true, portMAX_DELAY);
        printf("in end\n\r");

        break;
    }
    lcd_clear_screen(LCD_COLOR_BLACK);
    /* Print score based on reading eeprom's values */
    lcd_end_screen(eeprom_read_byte(eeprom_address), eeprom_read_byte(eeprom_address + 1));
}

void task_end_init(void)
{
    /* Create the End of Game Task */
    xTaskCreate(
        task_end,
        "End Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_end);
}