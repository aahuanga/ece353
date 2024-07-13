/**
 * @file task_game.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"
#include "project.h"
#include "project-lcd.h"
#include "task_game.h"
#include "task_buttons.h"
#include "task_light_sensor.h"
#include "task_eeprom.h"
#include "task_io_expander.h"

/*****************************************************************************/
/*  FreeRTOS Handles                                                         */
/*****************************************************************************/
TaskHandle_t TaskHandle_Game;

/**
 * @brief
 * This task display menu screen and switch game states. This controls the majority of task notifcations
 * to other tasks.
 *  @param pvParameters
 */
void task_game(void *Parameters)
{
    joystick_position_t prev_position = JOYSTICK_POS_CENTER;
    
    /* Draw the Main Menu Screen */
    lcd_start_llama(chosen_color);
    lcd_arrow(0);

    while (1)
    {
        /* Get joystick position to choose between start and choose color menu options*/
        button_state_t button = get_buttons();
        if (button == BUTTON_SW3_RELEASED)
        {
            eeprom_write_byte(0x0000, 0);
            eeprom_write_byte(0x0001, 0);
        }
        /* Get joystick position to choose between start and choose color menu options*/
        joystick_position_t position = joystick_get_pos();
        if (position == JOYSTICK_POS_DOWN && prev_position == JOYSTICK_POS_CENTER)
        {
            game = GAME_COLOR;
            lcd_arrow(1);
        }
        else if (position == JOYSTICK_POS_UP && prev_position == JOYSTICK_POS_CENTER)
        {
            game = GAME_START;
            lcd_arrow(0);
        }
        prev_position = position;

        /* Send task notification to ALS if user is hovering over change color*/
        if(game == GAME_COLOR)
        {
            xTaskNotifyGive(TaskHandle_light_sensor);

        }
        else if(game == GAME_PLAY)
        {
            break;
        }
   }
   while(1)
   {

   }
}

void task_game_init(void)
{
    /* Initalize Peripherals */
    ece353_enable_lcd();
    joystick_init();
    push_buttons_init();

    /* Create the Game Task */
    xTaskCreate(
        task_game,
        "Game Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_Game);
}