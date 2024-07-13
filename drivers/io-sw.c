/**
 * @file io-sw.c
 * @author Anna Huang ahuang62@wisc.edu
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "io-sw.h"

/*****************************************************************************/
/* ICE 02 START */
/*****************************************************************************/
void push_buttons_init(void)
{
    cy_rslt_t rslt_SW1;
    cy_rslt_t rslt_SW2;
    cy_rslt_t rslt_SW3;

    rslt_SW1 = cyhal_gpio_init(PIN_SW1, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    rslt_SW2 = cyhal_gpio_init(PIN_SW2, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    rslt_SW3 = cyhal_gpio_init(PIN_SW3, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    CY_ASSERT((rslt_SW1 == CY_RSLT_SUCCESS) && (rslt_SW2 == CY_RSLT_SUCCESS) && (rslt_SW3 == CY_RSLT_SUCCESS));
} 
/*****************************************************************************/
/* ICE 02 END */
/*****************************************************************************/


/*****************************************************************************/
/* ICE 03 START */
/*****************************************************************************/
/**
 * @brief
 * Returns which button has been pressed and then released. 
 *  
 * This function should examine the current state of each button and
 * compare it with the previous state of the button.  If the current
 * state is not-pressed and the previous state was pressed, return that
 * the button has been released.
 * 
 * If none of the buttons have been released, check to see if the button is 
 * currently being pressed.  If it is, return that the button is being pressed
 * 
 * If multiple buttons are pressed/released, return the button state of the button
 * with the highest SW number (SW3 has highest priority, then SW2, then SW1)
 * 
 * This function should only read the IN register once.
 * 
 * @return button_state_t 
 * Returns which of the buttons is currently being pressed.  
 */
button_state_t get_buttons(void)
{
    static bool sw1_curr = false;
    static bool sw1_prev = false;
    static bool sw2_curr = false;
    static bool sw2_prev = false;
    static bool sw3_curr = false;
    static bool sw3_prev = false;
    const volatile uint32_t led = PORT_BUTTONS->IN;

    if( (led & SW3_MASK) == 0){
        sw3_curr = true;
    } else if ((led & SW3_MASK) != 0){
        sw3_curr = false;
    } 
    if ( (led & SW2_MASK) == 0){
        sw2_curr = true;
    } else if( (led & SW2_MASK) != 0){
        sw2_curr = false;
    } 
    if ( (led & SW1_MASK) == 0){
        sw1_curr = true;
    } else if ( (led & SW1_MASK) != 0) {
        sw1_curr = false;
    }

    /* If the button was pressed (falling edge), turn blue LED ON*/
    if(sw3_curr) {
        sw3_prev = sw3_curr;
        return BUTTON_SW3_PRESSED;
    }

     /* If the button was released, turn blue OFF LED*/
    if(!sw3_curr && sw3_prev) {
        sw3_prev = sw3_curr;
        return BUTTON_SW3_RELEASED;
    }

     /* If the button was pressed (falling edge), turn green LED ON*/
    if(sw2_curr) {
        sw2_prev = sw2_curr;
        return BUTTON_SW2_PRESSED;
    }

     /* If the button was released, turn green OFF LED*/
    if(!sw2_curr && sw2_prev) {
        sw2_prev = sw2_curr;
        return BUTTON_SW2_RELEASED;
    }

   /* If the button was pressed (falling edge), turn red LED ON*/
    if(sw1_curr ) {
        sw1_prev = sw1_curr;
        return BUTTON_SW1_PRESSED;
    }

    /* If the button was released, turn red OFF LED*/
    if(!sw1_curr && sw1_prev) {
        sw1_prev = sw1_curr;
        return BUTTON_SW1_RELEASED;
    }

    return BUTTON_INACTIVE;
}

/*****************************************************************************/
/* ICE 03 END */
/*****************************************************************************/