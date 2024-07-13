/**
 * @file project-lcd.h
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __PROJECT_LCD_H__
#define __PROJECT_LCD_H__

#include "drivers/io-lcd.h"
#include "project.h"
#include "project-images.h"

void lcd_spit(uint16 x, uint16 y, int user);
void lcd_start_llama(uint32 llama_color);
void lcd_arrow(bool joystick_option);
void lcd_end_screen(int player0, int player1);

#endif
