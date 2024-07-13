/**
 * @file hw01_timer.c
 * @author Robin Yang(ryang264@wisc.edu), Anna Huang (ahuang62@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "hw01_timer.h"
/*****************************************************************************/
/*  Function Declaration                                                     */
/*****************************************************************************/
void Handler_HW01_Timer(void *handler_arg, cyhal_timer_event_t event);

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
//Handler alert variables for time update, button press and clock blink
hw01_timer_alerts_t alert_update = HW01_ALERT_NONE;
hw01_timer_alerts_t alert_sw3 = HW01_ALERT_NONE;
hw01_timer_alerts_t alert_sw1 = HW01_ALERT_NONE;
hw01_timer_alerts_t alert_blink = HW01_ALERT_NONE;
/* Timer object and timer_cfg object */
cyhal_timer_t timer_obj;

//Period set in main
cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,              /* Timer compare value, not used */
        .period = 0,          /* number of timer ticks */
        .direction = CYHAL_TIMER_DIR_UP, /* Timer counts up */
        .is_compare = false,             /* Don't use compare mode */
        .is_continuous = true,           /* Run timer indefinitely */
        .value = 0                       /* Initial value of counter */
};

/*****************************************************************************/
/*  Interrupt Handlers                                                       */
/*****************************************************************************/

/**
 * @brief
 *  Function used as Timer Handler
 */
void Handler_HW01_Timer(void *handler_arg, cyhal_timer_event_t event)
{
    CY_UNUSED_PARAMETER(handler_arg);
    CY_UNUSED_PARAMETER(event);

    //Static counters for update and blink
    static int counter = 0;
    //Static counter for sw1 and sw3 
    static int sw3_counter = 0;
    static int sw1_counter = 0;
    uint32_t reg_val = PORT_BUTTONS->IN;

    //Always increment counter every handler call, reset at 10
    counter = (counter+1)%10;

    //every second update time
    if (counter == 0)
    {
        alert_update = HW01_ALERT_TIME_UPDATE;
    }

    //for half a second assert blink, other half assert none
    if (counter < 5)
    {
        alert_blink = HW01_ALERT_BLINK;
    }
    else
    {
        alert_blink = HW01_ALERT_NONE;
    }

    //If sw3 is pressed start counting
    if ( (reg_val & SW3_MASK) == 0 )
    {
        sw3_counter++;
    }
    //if not currently pressed but previously pressed, indicated by sw3counter, assert appropriate duration 
    else if ( (sw3_counter > 0) && (reg_val & SW3_MASK) != 0 )
    {
        if ( sw3_counter <= 20 )
        {
            alert_sw3 = HW01_ALERT_BUTTON_LT_2S;
        }
        else
        {
            alert_sw3 = HW01_ALERT_BUTTON_GT_2S;
        }
        sw3_counter = 0;
    }
  
    //if alert_sw1 has been set to alert_blink from hw01.c (arbitrary value used to indicate alarm is ringing), and sw1 is pressed, increment 
    if ( (alert_sw1 == HW01_ALERT_BLINK) &&  ((reg_val & SW1_MASK) == 0) )
    {
        sw1_counter++;
    }
    // if sw1 is not being pressed and counter is greater than 20 (2s) assert greater than 2s
    else if( (reg_val & SW1_MASK) != 0 && sw1_counter >= 20)
    {
        alert_sw1 = HW01_ALERT_BUTTON_GT_2S;
        sw1_counter = 0;
    }
    else
    {
        sw1_counter = 0;
    }

}

//Initalization code from ice
/*****************************************************************************/
/*  TCPWM Initialization Function                                            */
/*****************************************************************************/
void hw01_timer_init(uint32_t ticks)
{
    timer_cfg.period = ticks;

    cy_rslt_t rslt;

    /* Initialize a timer */
    rslt = cyhal_timer_init(&timer_obj, NC, NULL);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization fails, halt the MCU

    /* Apply timer configuration such as period, count direction, run mode, etc. */
    rslt = cyhal_timer_configure(&timer_obj, &timer_cfg);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization fails, halt the MCU

    /* Set the frequency of timer*/
    rslt = cyhal_timer_set_frequency(&timer_obj, 100000000);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization fails, halt the MCU

    /* Assign the ISR to execute on timer interrupt */
    cyhal_timer_register_callback(&timer_obj, Handler_HW01_Timer, NULL);

    /* Set the event on which timer interrupt occurs and enable it */
    cyhal_timer_enable_event(&timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);

    /* Start the timer with the configured settings */
    rslt = cyhal_timer_start(&timer_obj);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization fails, halt the MCU
}
