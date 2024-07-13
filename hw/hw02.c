/**
 * @file hw01.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"
#include "hw02.h"
#include "hw02-lcd-staff.h"
#include "hw02-images.h"

#define TICKS_MS_10 (1000000) // 10ms

typedef enum
{
    ALERT_TIMER_NONE,
    ALERT_TIMER_100MS,
    ALERT_TIMER_SW1,
    ALERT_TIMER_SW2
} alert_timer_t;

struct square_t
{
    int player;
    int active;
    int claimed;
};
/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
char HW02_DESCRIPTION[] = "ECE353: HW02 Robin Yang, Anna Huang";

alert_timer_t ALERT_TIMER = ALERT_TIMER_NONE;
alert_timer_t ALERT_SW1 = ALERT_TIMER_NONE;
alert_timer_t ALERT_SW2 = ALERT_TIMER_NONE;

/* Timer object used */
cyhal_timer_t hw02_timer_obj;
cyhal_timer_cfg_t hw02_timer_cfg;

extern cyhal_uart_t remote_uart_obj;
uint8_t PLAYER1_SELECTION = 0x5A;
uint8_t PLAYER_ACK = 0xF0;
uint8_t SELECT_X = 0x58;
uint8_t SELECT_O = 0x4F;

struct square_t board[3][3];
int start = 0; // Indicates if you are the starting player (1 if you are starting, 0 if you are second player)

/*****************************************************************************/
/*  Interrupt Handlers                                                       */
/*****************************************************************************/
/**
 * @brief
 * Function used as Timer Handler
 */
void Handler_Timer(void *handler_arg, cyhal_timer_event_t event)
{
    uint32_t reg_val = PORT_BUTTONS->IN;
    // Static counter for joystick alert, sw1, and sw2
    static uint8_t alert_count = 0;
    static uint8_t sw1_count = 0;
    static uint8_t sw2_count = 0;

    // Always increment counter every handler call, reset at 10
    alert_count = (alert_count + 1) % 10;

    // Assert timer interrupt for joystick at 100ms
    if (alert_count == 0)
    {
        ALERT_TIMER = ALERT_TIMER_100MS;
    }

    // Assert timer interrupt for sw1 if equal or greater 100ms and not pressed
    if ((reg_val & SW1_MASK) == 0)
    {
        sw1_count = (sw1_count + 1);
    }
    else if (sw1_count >= 10 && (reg_val & SW1_MASK) != 0)
    {
        ALERT_SW1 = ALERT_TIMER_SW1;
        sw1_count = 0;
    }
    else
    {
        sw1_count = 0;
    }
    // Assert timer interrupt for sw2 if equal or greater 100ms and not pressed
    if ((reg_val & SW2_MASK) == 0)
    {
        sw2_count = (sw2_count + 1);
    }
    else if (sw2_count >= 10 && (reg_val & SW2_MASK) != 0)
    {
        ALERT_SW2 = ALERT_TIMER_SW2;
        sw2_count = 0;
    }
    else
    {
        sw2_count = 0;
    }
}

/*****************************************************************************/
/*  HW02 Functions                                                           */
/*****************************************************************************/

/**
 * @brief
 * Initializes the PSoC6 Peripherals used for HW01
 */
void hw02_peripheral_init(void)
{
    /* Initialize the pushbuttons */
    push_buttons_init();

    /* Initialize the LCD */
    ece353_enable_lcd();

    /* Initialize the joystick*/
    joystick_init();

    /* Initialize the remote UART */
    remote_uart_init();

    /* Enable interrupts and circular buffers from remote board.*/
    remote_uart_enable_interrupts();

    /* Initialize Timer to generate interrupts every 10mS*/
    timer_init(&hw02_timer_obj, &hw02_timer_cfg, TICKS_MS_10, Handler_Timer);
}

/**
 * @brief
 * Draws initial tic tac toe board
 */
void tic_tac_toe_start(void)
{
    // Erase the ending screen of a game when reset
    lcd_clear_screen(LCD_COLOR_BLACK);

    tic_tac_toe_draw_grid();
}

/**
 * @brief
 * Checks if a player wins
 */
int check_board(void)
{
    int win = -1;

    // Check rows and column wins
    for (int i = 0; i < 3; i++)
    {
        int count_row = 0;
        int count_col = 0;
        for (int j = 0; j < 3; j++)
        {
            // Check if the square is claimed by a player and increment count if it is the same player (1 for X and -1 for O)
            if (board[i][j].claimed)
            {
                count_row += (board[i][j].player ? -1 : 1);
            }
            if (board[j][i].claimed)
            {
                count_col += (board[j][i].player ? -1 : 1);
            }
        }
        // count up rows for either player 0 or 1
        if (count_row == 3 || count_col == 3)
        {
            win = 0;
        }
        else if (count_row == -3 || count_col == -3)
        {
            win = 1;
        }
    }

    // Initalize starting column to 2
    int j = 2;
    int count_diag = 0;
    int count_diag2 = 0;

    // check diagonals of board
    for (int i = 0; i < 3; i++)
    {
        // Check if the square is claimed by a player and increment count if it is the same player (1 for X and -1 for O)
        if (board[i][i].claimed)
        {
            count_diag += (board[i][i].player ? -1 : 1);
        }
        if (board[i][j - i].claimed)
        {
            count_diag2 += (board[i][j - i].player ? -1 : 1);
        }
        // check if player 0 or 1 has 3 in a row
        if (count_diag == 3 || count_diag2 == 3)
        {
            win = 0;
        }
        else if (count_diag == -3 || count_diag2 == -3)
        {
            win = 1;
        }
    }

    return win;
}

/**
 * @brief
 * Draws each square of the tic tac toe board when called
 * @param col Column of the tic tac toe board
 * @param row Row of the tic tac toe board
 * @param player States whether to place X (0) or O (1)
 * @param active States whether the square is active (1) or not (0) based on the joystick position
 * @param claimed States whether the square is claimed (1) or not (0)
 * @param current_player States if the current player is X (0) or O (1)
 */
void draw_tic_tac_toe(int col, int row, int player, int active, int claimed, int current_player)
{
    // Initalize drawing variables
    int foreground;
    int width;
    int height;
    int bitmap;
    int background;

    if (active)
    {
        if (claimed)
        {
            // If the square is active & claimed, set the bitmap to 0 (X) or 1 (O)
            // based on the player who claimed it and background color to red
            bitmap = (player ? 1 : 0);
            background = BG_COLOR_CLAIMED;
        }
        else
        {
            // If the square is active & not claimed, set the bitmap to 0 (X) or 1 (O)
            // based on the player who claimed it and background color to green
            bitmap = (current_player ? 1 : 0);
            background = BG_COLOR_UNCLAIMED;
        }
        foreground = LCD_COLOR_BLACK;
    }
    else
    {
        if (claimed)
        {
            // If the square is not active & claimed, set the letter color based on the player
            foreground = (player ? FG_COLOR_O : FG_COLOR_X);
        }
        else
        {
            // If the square is not active & not claimed, set the letter color to black
            foreground = LCD_COLOR_BLACK;
        }
        bitmap = (player ? 1 : 0);
        background = LCD_COLOR_BLACK;
    }

    // Checks if the bitmap should be X or O and set the width and height accordingly
    if (bitmap == 1)
    {
        width = O_WIDTH;
        height = O_HEIGHT;
    }
    else
    {
        width = X_WIDTH;
        height = X_HEIGHT;
    }

    // Set row variable to the correct board row position
    if (row == 0)
    {
        row = UPPER_ROW;
    }
    else if (row == 1)
    {
        row = CENTER_ROW;
    }
    else
    {
        row = LOWER_ROW;
    }

    // Set col variable to the correct board column position
    if (col == 0)
    {
        col = LEFT_COL;
    }
    else if (col == 1)
    {
        col = CENTER_COL;
    }
    else
    {
        col = RIGHT_COL;
    }

    // Draw the background square color
    lcd_draw_rectangle_centered(col, row, SQUARE_SIZE, SQUARE_SIZE, background);

    // Draws the X or 0 depending on the bitmap
    if (bitmap == 1)
    {
        lcd_draw_image(col, row, width, height, Bitmaps_O, foreground, background);
    }
    else
    {
        lcd_draw_image(col, row, width, height, Bitmaps_X, foreground, background);
    }
}

/**
 * @brief
 * sents message passed in from arg, waits spin waits for ack to be received, then returns
 * @param message
 */
void uart_send(uint8_t message)
{
    /* Sends byte char*/
    remote_uart_tx_char_async(message);
    remote_uart_tx_char_async('\n');

    while (1)
    {
        if (ALERT_UART_RX)
        {
            /* Clear out the array holding the message */
            ALERT_UART_RX = false;

            /* Clears messgae byte received before*/
            message = 0;

            /* Get byte that was recieved */
            remote_uart_rx_data_async(&message, 1);

            /* Check if message is acknowledged */
            if (message == PLAYER_ACK)
            {
                return;
            }
        }
    }
}

/**
 * @brief
 * received message, mode 0 checks for player selection, mode 1 checks for O or X selection, else checks for player placement
 * if received, send ack and returned true. else return false
 * @param mode
 * @param rx
 * @return true
 * @return false
 */
bool uart_receive(int mode, uint8_t *rx)
{
    /* Initialize condition to see if an acknowldge byte was recieved and message byte*/
    bool condition;
    uint8_t message = 0;

    /* Get byte that was recieved */
    remote_uart_rx_data_async(&message, 1);

    /* Checks if the byte is acknowledge based on modes:
    *  Mode 0 for a player selection byte
    *  Mode 1 for a letter selection byte
    *  Mode 2 for a square selection byte
    */
    if (mode == 0)
    {
        condition = (message == PLAYER1_SELECTION);
    }
    else if (mode == 1)
    {
        condition = (message == 0x58 || message == 0x4F);
    }
    else
    {
        condition = (message == 0x00 || message == 0x01 || message == 0x02 || message == 0x10 ||
                     message == 0x11 || message == 0x12 || message == 0x20 || message == 0x21 || message == 0x22); // All possible square selection bytes
    }

    /*  Save message sent into rx */
    *rx = message;

    /* If it is an acknowledge byte, send byte to remote board */
    if (condition)
    {
        /* Send the String to the remote board */
        remote_uart_tx_char_async(PLAYER_ACK);

        /* Need to send a \r or \n to indicate the end of a string*/
        remote_uart_tx_char_async('\n');
        return true;
    }
    return false;
}

/**
 * @brief
 * Draws initial tic tac toe board
 */
void gameplay(int player)
{
    // Initialize joystick positions, button state, and edit_flag indicator
    joystick_position_t curr_position = JOYSTICK_POS_CENTER;
    joystick_position_t prev_position = JOYSTICK_POS_CENTER;
    int edit_flag = 1;    // Indicates whether or not the array has changed
    int disable_flag = 0; // Indicates when the game is finished and should disable movement
    int sw1_count = 0;
    int swap = 0; // Indicates when to change boards

    // Initialize col and row to the center square of the tic tac toe board
    int col = 1;
    int row = 1;

    // Initalize all variables in the board 2D to 0
    memset(board, 0, (size_t)9 * sizeof(struct square_t));
    tic_tac_toe_start();
    if (start)
    {
        // Set the center square as active
        board[1][1].active = 1;
        // Draw inital tic tac toe board
    }

    while (1)
    {
        /* Check if the player is the starting player */
        if (start)
        {
            edit_flag = 1;

            // Starting player loop
            while (1)
            {
                // If edit_flag is true, draw each square of the board with the correct parameters
                if (edit_flag)
                {
                    // Set edit flag back to false
                    edit_flag = 0;
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            draw_tic_tac_toe(j, i, board[i][j].player, board[i][j].active, board[i][j].claimed, player);
                        }
                    }

                    // Check win condition and print appropriate message
                    int win = check_board();
                    if (win == -1 && sw1_count == 9)
                    {
                        lcd_tie();
                        disable_flag = 1;
                    }
                    else if (win == 0)
                    {
                        lcd_X_wins();
                        disable_flag = 1;
                    }
                    else if (win == 1)
                    {
                        lcd_O_wins();
                        disable_flag = 1;
                    }
                    /* Checks if playing board should be changed to other board*/
                    if (swap)
                    {
                        // Create square selection byte based on the row and col of the square
                        uint8_t pos = row * 16 + col;

                        // Send the byte to remote board
                        uart_send(pos);

                        // Set swap and starting player to 0
                        swap = 0;
                        start = 0;

                        // Check if disable flag is active (meaning the game is finished)
                        if (!disable_flag)
                        {
                            // Break from starting player loop
                            break;
                        }
                    }
                }

                // Handle joystick alert
                if (ALERT_TIMER == ALERT_TIMER_100MS)
                {
                    // Set interrupt flag back to false
                    ALERT_TIMER = ALERT_TIMER_NONE;

                    if (disable_flag)
                    {
                        goto endgame;
                    }
                    // Get the current position
                    curr_position = joystick_get_pos();

                    // If the joystick was moved and was from the center position, determine the position of the board
                    if (curr_position != prev_position && prev_position == JOYSTICK_POS_CENTER)
                    {
                        // Set the board square (that you are moving away from) inactive
                        board[row][col].active = 0;

                        // Checks the joystick position and updates the row and column
                        switch (curr_position)
                        {
                        case JOYSTICK_POS_UP:
                        {
                            // Decrement the row, and account for wrapping from row 0 to row 2 (by adding 3 and modulus by 3)
                            row = (3 + row - 1) % 3;
                            break;
                        }
                        case JOYSTICK_POS_DOWN:
                        {
                            // Increment the row, and account for wrapping from row 2 to row 0 (modulus by 3)
                            row = (row + 1) % 3;
                            break;
                        }
                        case JOYSTICK_POS_LEFT:
                        {
                            // Decrement the col, and account for wrapping from col 0 to col 2 (by adding 3 and modulus by 3)
                            col = (3 + col - 1) % 3;
                            break;
                        }
                        case JOYSTICK_POS_RIGHT:
                        {
                            // Increment the col, and account for wrapping from col 2 to col 0 (modulus by 3)
                            col = (col + 1) % 3;
                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }

                        // Set new board square to active
                        board[row][col].active = 1;
                        // Set edit flag to true
                        edit_flag = 1;
                    }

                    // Save the previous position
                    prev_position = curr_position;
                }

                // If button sw1 is release, claim the current square (if not already claimed)
                if (ALERT_SW1 == ALERT_TIMER_SW1)
                {
                    ALERT_SW1 = ALERT_TIMER_NONE;
                    // Checks if the board square is not claimed
                    if (!board[row][col].claimed)
                    {
                        // Set the square to be claimed by the current player
                        board[row][col].claimed = 1;
                        board[row][col].player = player;
            
                        // Set edit flag to true
                        edit_flag = 1;

                        //Increment sw1_count and change swap indicator
                        sw1_count++;
                        swap++;
                        
                    }
                }

            endgame:
                // If button sw2 is release, return to main_app function to restart the game
                if (ALERT_SW2 == ALERT_TIMER_SW2)
                {
                    ALERT_SW2 = ALERT_TIMER_NONE;
                    start = 1;
                    uart_send(PLAYER1_SELECTION);
                    return;
                }

                // Checks for uart alert
                if (ALERT_UART_RX)
                {
                    uint8_t rx; // Initialize rx

                    /* Set the alert back to false*/
                    ALERT_UART_RX = false;
                    
                    /* Check if uart signal is recieved*/
                    if (uart_receive(0, &rx))
                    {
                        /* Set player to not starting player */
                        start = 0;
                        return;
                    }
                }
            }
        }
        else
        {
            // Draw waiting splash screen
            lcd_wait_for_other_player();
            while (1)
            {
                uint8_t rx;
                if (ALERT_UART_RX)
                {
                    /* Set the alert back to false*/
                    ALERT_UART_RX = false;
                    // Checks if square selection byte is received
                    if (uart_receive(2, &rx))
                    {
                        // Set previous square to inactive
                        board[row][col].active = 0;

                        // Set player to starting player
                        start = 1;

                        // Clear screen splash screen
                        lcd_clear_other_player();

                        // Calculate row and col based on byte received
                        row = rx / 16;
                        col = rx % 16;
                        
                        // Set tic tac toe board for remote board
                        board[row][col].claimed = 1;
                        board[row][col].player = (player + 1) % 2;
                        board[row][col].active = 1;
                        sw1_count++;
                        break;
                    }
                }

                if (ALERT_SW2 == ALERT_TIMER_SW2)
                {
                    // Set sw2 alert to none
                    ALERT_SW2 = ALERT_TIMER_NONE;
                    start = 1;

                    // Send player1 selection byte for resetting game and return to main loop
                    uart_send(PLAYER1_SELECTION);
                    return;
                }
            }
        }
    }
}

int gameinit()
{
    lcd_select_player1();

    int player_start = 0;

    /* Clear out the array holding the message */
    while (1)
    {
        if (ALERT_SW2 == ALERT_TIMER_SW2)
        {
            // Set interrupt flag back to false
            ALERT_SW2 = ALERT_TIMER_NONE;

            uart_send(PLAYER1_SELECTION);
            break;
        }

        if (ALERT_UART_RX)
        {
            /* Set the alert back to false*/
            ALERT_UART_RX = false;
            int rand; // placeholder receiving variable
            if (uart_receive(0, &rand))
            {
                player_start = 1;
                break;
            }
        }
    }
    int choose = 0; // Indicates which letter player chooses

    tic_tac_toe_start();

    // Checks if they are first player
    if (player_start == 0)
    {
        draw_tic_tac_toe(1, 1, 0, 1, 0, choose); // Draws starting tic tac toe board
        // Player who starts is 1
        start = 1;
        while (1)
        {
            if (ALERT_SW1 == ALERT_TIMER_SW1)
            {
                choose = (choose + 1) % 2;
                ALERT_SW1 = ALERT_TIMER_NONE;
                draw_tic_tac_toe(1, 1, 0, 1, 0, choose); // Draw with different selected letter
            }
            if (ALERT_SW2 == ALERT_TIMER_SW2)
            {
                ALERT_SW2 = ALERT_TIMER_NONE;
                int select = (choose ? 0x58 : 0x4F);

                // Send letter selection byte
                uart_send(select);
                break;
            }
        }
    }
    else
    {
        // player who goes second is 0
        start = 0;
        lcd_wait_for_other_player();
        uint8_t rx;
        while (1)
        {
            if (ALERT_UART_RX)
            {
                /* Set the alert back to false*/
                ALERT_UART_RX = false;
                if (uart_receive(1, &rx))
                {
                    // Determines which letter the player is based on receiving byte
                    choose = ((rx == 0x58) ? 0 : 1);
                    break;
                }
            }
        }
    }

    return choose;
}

/**
 * @brief
 * Implements the main application for HW02
 */
void hw02_main_app(void)
{
    // Initialize player to X
    uint8_t player = 0;

    player = gameinit();
    while (1)
    {
        gameplay(player);
    }
}