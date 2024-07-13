/**
 * @file project.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __PROJECT_H__
#define __PROJECT_H__

#include "main.h"
#include "project-images.h"
#include "project-lcd.h"
#include "task_buttons.h"
#include "task_eeprom.h"
#include "task_io_expander.h"
#include "task_light_sensor.h"
#include "task_IMU.h"
#include "task_UART.h"
#include "task_LCD.h"
#include "task_game.h"
#include "task_end.h"
#include "circular_buffer_struct.h"

typedef enum {
    GAME_START,
    GAME_COLOR,        /* A button was pressed for more than 2 seconds */
    GAME_PLAY,
    GAME_END,
}game_state_t;

extern char PROJECT_DESCRIPTION[];
extern int chosen_color;
extern int player;
extern int x_coord;
extern int y_coord;
extern game_state_t game;

extern TaskHandle_t TaskHandle_SW1;
extern TaskHandle_t TaskHandle_SW2;
extern TaskHandle_t TaskHandle_LCD;
/**
 * @brief
 * Initializes the PSoC6 Peripherals used for PROJECT
 */
void project_peripheral_init(void);

/**
 * @brief
 * Implements the main application for PROJECT
 */
void project_main_app(void);

#endif
