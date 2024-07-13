/**
 * @file project-lcd.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "project-lcd.h"

/*******************************************************************************
 * Function Name: lcd_spit
 ********************************************************************************
 * Summary: Prints a llama spit image
 * Params: x - x coordinate of the llama's spit
 *         y - y coordinate of the llama's spit
 * Returns:
 *  Nothing
 *******************************************************************************/
void lcd_spit(uint16 x, uint16 y, int user)
{
    if (user){
        lcd_draw_image(
            x, // 48
            y, // 200
            spitWidthPixels,
            spitHeightPixels,
            spitReverseBitmaps,
            chosen_color,
            LCD_COLOR_BLACK);
    } else{
        lcd_draw_image(
            x, 
            y, 
            spitWidthPixels,
            spitHeightPixels,
            spitBitmaps,
            chosen_color,
            LCD_COLOR_BLACK);
    }
}

/*******************************************************************************
 * Function Name: lcd_start_llama
 ********************************************************************************
 * Summary: Prints a menu screen llama image
 * Params: llama_color - the color of the llama that user chose
 * Returns:
 *  Nothing
 *******************************************************************************/
void lcd_start_llama(uint32 llama_color)
{
    lcd_draw_image(
        240,
        140,
        startLlamaWidthPixels,
        startLlamaHeightPixels,
        startLlamaBitmaps,
        llama_color,
        LCD_COLOR_BLACK);
};

/*******************************************************************************
 * Function Name: lcd_arrow
 ********************************************************************************
 * Summary: Prints a menu arrow image and the start and change color text
 * Params: joystick_option - joystick positions (0 for up, 1 for down)
 * Returns:
 *  Nothing
 *******************************************************************************/
void lcd_arrow(bool joystick_option)
{
    if (joystick_option)
    {
        lcd_draw_image(
            40,
            82,
            arrowWidthPixels,
            arrowHeightPixels,
            arrowBitmaps,
            LCD_COLOR_BLACK,
            LCD_COLOR_BLACK);
        lcd_draw_image(
            90,
            80,
            startWidthPixels,
            startHeightPixels,
            startBitmaps,
            LCD_COLOR_WHITE,
            LCD_COLOR_BLACK);
        lcd_draw_image(
            97,
            140,
            changeWidthPixels,
            changeHeightPixels,
            changeBitmaps,
            LCD_COLOR_YELLOW,
            LCD_COLOR_BLACK);
        lcd_draw_image(
            40,
            130,
            arrowWidthPixels,
            arrowHeightPixels,
            arrowBitmaps,
            LCD_COLOR_YELLOW,
            LCD_COLOR_BLACK);
    }
    else
    {
        lcd_draw_image(
            40,
            130,
            arrowWidthPixels,
            arrowHeightPixels,
            arrowBitmaps,
            LCD_COLOR_BLACK,
            LCD_COLOR_BLACK);
        lcd_draw_image(
            90,
            80,
            startWidthPixels,
            startHeightPixels,
            startBitmaps,
            LCD_COLOR_YELLOW,
            LCD_COLOR_BLACK);
        lcd_draw_image(
            97,
            140,
            changeWidthPixels,
            changeHeightPixels,
            changeBitmaps,
            LCD_COLOR_WHITE,
            LCD_COLOR_BLACK);
        lcd_draw_image(
            40,
            82,
            arrowWidthPixels,
            arrowHeightPixels,
            arrowBitmaps,
            LCD_COLOR_YELLOW,
            LCD_COLOR_BLACK);
    }
};

/*******************************************************************************
 * Function Name: lcd_end_screen
 ********************************************************************************
 * Summary: Prints a end score 
 * Params: player0 - player 0's win score
 *         player1 - player 1's win score
 * Returns:
 *  Nothing
 *******************************************************************************/
void lcd_end_screen(int player0, int player1)
{
    lcd_draw_rectangle(145,30,115,18,LCD_COLOR_WHITE);

    switch (player0)
    {
        case 0:
        {
            lcd_draw_image(
                70,
                120,
                numberWidthPixels,
                numberHeightPixels,
                zeroBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
        case 1:
        {
            lcd_draw_image(
                70,
                120,
                numberWidthPixels,
                numberHeightPixels,
                oneBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
        case 2:
        {
            lcd_draw_image(
                70,
                120,
                numberWidthPixels,
                numberHeightPixels,
                twoBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
        case 3:
        {
            lcd_draw_image(
                70,
                120,
                numberWidthPixels,
                numberHeightPixels,
                threeBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
        case 4:
        {
            lcd_draw_image(
                70,
                120,
                numberWidthPixels,
                numberHeightPixels,
                fourBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
    }
    switch (player1)
    {
        case 0:
        {
            lcd_draw_image(
                250,
                120,
                numberWidthPixels,
                numberHeightPixels,
                zeroBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
        case 1:
        {
            lcd_draw_image(
                250,
                120,
                numberWidthPixels,
                numberHeightPixels,
                oneBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
        case 2:
        {
            lcd_draw_image(
                250,
                120,
                numberWidthPixels,
                numberHeightPixels,
                twoBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
        case 3:
        {
            lcd_draw_image(
                250,
                120,
                numberWidthPixels,
                numberHeightPixels,
                threeBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
        case 4:
        {
            lcd_draw_image(
                250,
                120,
                numberWidthPixels,
                numberHeightPixels,
                fourBitmaps,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK);
            break;
        }
    }
};