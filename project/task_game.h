/**
 * @file task_game.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __TASK_GAME_H__
#define __TASK_GAME_H__

#include "main.h"

extern TaskHandle_t TaskHandle_Game;

void task_game(void *Parameters);
void task_game_init(void);

#endif