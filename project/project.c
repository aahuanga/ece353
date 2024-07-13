/**
 * @file project.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "project.h"


/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
char PROJECT_DESCRIPTION[] = "ECE353: PROJECT Robin Yang, Anna Huang";
int chosen_color = LCD_COLOR_WHITE; // Default Llama color
int player = 0; // Sets which player the user is
int x_coord = 280; // Game Llama x Coordinates
int y_coord = 200; // Game Llama y Coordinates

// Game Mode State
game_state_t game = GAME_START;

/*****************************************************************************/
/* Peripheral Initialization                                                  */
/*****************************************************************************/
/**
 * @brief
 * Initializes the PSoC6 Peripherals used for HW01
 */
void project_peripheral_init(void)
{
    /* No Code Here */
}

/**
 * @brief
 * Implements the main application for PROJECT
 */
void project_main_app(void)
{   
    // Initalize all tasks
    task_light_sensor_init();
    task_game_init();
    task_buttons_init();
    task_LCD_init();
    task_IMU_init();
    task_io_expander_init();
    task_eeprom_init();
    task_UART_init();
    task_end_init();
    vTaskStartScheduler();

    while (1)
    {

    }
}