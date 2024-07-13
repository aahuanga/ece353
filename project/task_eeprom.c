/**
 * @file task_eeprom.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "task_eeprom.h"

/*****************************************************************************/
/*  FreeRTOS Handles                                                         */
/*****************************************************************************/
TaskHandle_t TaskHandle_EEPROM;

/**
 * @brief
 * This task will write to EEPROM.  When LCD is at End Game, the task will write to EEPROM
 * and give a task notification to the end task to update the score.
 *  @param pvParameters
 */
void task_eeprom(void *Parameters)
{
    uint16_t address = 0x0000; // Set address of score to 0

    BaseType_t win_status;
    uint16_t win;
    uint8_t Write_Values[2];

    while (1)
    {
        // Wait to recieve from queue to get win value to see which player won the game
        win_status = xQueueReceive(Win_Queue, &win, 5);
        if (win_status)
        {
            printf("in eeprom\n\r");
            // Recieve which player wins and increment the player's win score
            if (win == 0)
            {
                Write_Values[0] = (eeprom_read_byte(address) + 1) % 5;
            }
            else
            {
                Write_Values[1] = (eeprom_read_byte(address + 1) + 1) % 5;
            }

            // Writing Values to EEPROM
            for (int i = 0; i < 2; i++)
            {
                eeprom_write_byte(address, Write_Values[i]);
                address++;
            }
            address = 0;

            // Writing Values to EEPROM
            for (int i = 0; i < 2; i++)
            {
                if (eeprom_read_byte(i) < 5 && eeprom_read_byte(i) >= 0)
                {
                    // Do nothing
                    printf("%i\n\r", eeprom_read_byte(i));
                }
                else
                {
                    eeprom_write_byte(i, 0);
                }
            }

            xTaskNotifyGive(TaskHandle_end);
        }
    }
}

void task_eeprom_init(void)
{
    /* Initialize peripherals */
    eeprom_cs_init();

    /* Create the EEPROM Task */
    xTaskCreate(
        task_eeprom,
        "EEPROM Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_EEPROM);
}