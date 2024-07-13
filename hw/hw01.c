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
//Finished
#include "hw01.h"
#include "main.h"

#define TICKS_MS_100  (10000000) //100ms
/*****************************************************************************/
/*  Function Declaration                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
//Keep track time between modes
uint8_t clock_minutes = 0;
uint8_t clock_seconds = 0;
uint8_t alarm_minutes = 0;
uint8_t alarm_seconds = 0;

/**
 * @brief
 * Initializes the PSoC6 Peripherals used for HW01
 */
void hw01_peripheral_init(void)
{
    /* Initialize the pushbuttons */
    push_buttons_init();

    /* Initalize the LEDs */
    leds_init();

    /* Initialize the LCD */
    ece353_enable_lcd();

    /* Initialize the buzzer */
    pwm_buzzer_init();

    /* Intitialize a timer to generate an interrupt every 100mS*/
    hw01_timer_init(TICKS_MS_100); 
}

/**
 * @brief 
 * Edit alarm or clock minutes and seconds
 * @param minutes current minute value
 * @param seconds current seconds value 
 * @param sec_flag if 0 edit minutes, else edit seconds 
 * @param dec_flag if 0 inc , else  dec 
 * @param mode  mode that is running 
 */
void edit_clock(uint8_t minutes, uint8_t seconds, int sec_flag, int dec_flag, hw01_mode mode)
{
    //set minutes
    if( sec_flag == 0 )
    {
        //increment minutes
        if ( dec_flag == 0)
        {
            //wrap around to 0 if minutes is going to be greater than 2
            if (minutes+1 > 2)
            {
                minutes = 0;
            }
            //else just increment
            else
            {
                minutes++;
            }
        }
        //decrement minutes
        else
        {
            //wrap arond to 2 if minutes is going to be less than 0
            if(minutes-1 < 0)
            {
                minutes = 2;
            }
            //else just decrement
            else
            {
                minutes--;
            }
        }
        
    }
    //set seconds
    else
    {
        //increment seconds
        if ( dec_flag == 0)
        {
            //wrap around to 0 if seconds is going to be greater than 59
            if (seconds+1 > 59)
            {
                //if run mode, and wrapping around, set minutes to 0 if currently at 2 minutes, else just increment minutes
                if (mode == RUN_MODE)
                {
                    if(minutes == 2)
                    {
                        minutes = 0;
                    }
                    else
                    {
                        minutes++;
                    }
                }
                //reset seconds to 0
                seconds = 0;

            }
            //if not wrapping around just increment seconds
            else
            {
                seconds++;
            }
        }
        //decrement seconds
        else
        {
            //if seconds is going to be less than 0, wrap to 59
            if (seconds-1 < 0)
            {
                seconds = 59;
            }
            //else decrement
            else
            {
                seconds--;
            }
        }
        
    }

    if (mode == SET_ALARM_MODE)
    {
        alarm_minutes = minutes;
        alarm_seconds = seconds;
    }
    else
    {   clock_minutes = minutes;
        clock_seconds = seconds;
    }

}

/**
 * @brief Set the mode object
 * continuously update time and check buttons to see if time should be changed
 * @param mode which mode
 */
void set_mode(hw01_mode mode)
{
    //initalize button_state_t for sw1 and sw2
    button_state_t button = BUTTON_INACTIVE;
    //indicate which should be edited
    int sec_flag = 0;

    //continuously loop
    while(1)
    {
    
        button = get_buttons();   
        //if in set alarm mode and update is asserted, call edit clock to increment by 1 second in run time, display new time 
        if ( (mode == SET_ALARM_MODE) && alert_update== HW01_ALERT_TIME_UPDATE)
        {
            //sec flag is 1 and dec flag is 0
            edit_clock(clock_minutes, clock_seconds, 1, 0, RUN_MODE);
            hw01_display_time(clock_minutes, clock_seconds, HW01_LCD_TIME_COLOR); 
            alert_update = HW01_ALERT_NONE; 
        }

        //if sw3 is asserted as lt_2s, then flip sec_flag 
        if ( alert_sw3 == HW01_ALERT_BUTTON_LT_2S )
        {
            sec_flag = ~sec_flag;
            //clear assert
            alert_sw3 = HW01_ALERT_NONE;
        }

        //if sw1 released increment with current sec_flag 
        if ( button == BUTTON_SW1_RELEASED)
        {
            //if in time mode edit clock, if in alarm mode edit alarm
            if (mode == SET_TIME_MODE)
            {
                edit_clock(clock_minutes, clock_seconds, sec_flag, 0, mode);
            }
            else if(mode == SET_ALARM_MODE)
            {
                edit_clock(alarm_minutes, alarm_seconds, sec_flag, 0, mode);
            }
        }
        //if sw2 released decrement with current sec_flag
        else if( button == BUTTON_SW2_RELEASED)
        {
           if (mode == SET_TIME_MODE)
            {
                edit_clock(clock_minutes, clock_seconds, sec_flag, 1, mode);
            }
            else
            {
                edit_clock(alarm_minutes, alarm_seconds, sec_flag, 1, mode);
            }
        }

        //If in set time moe blink clock, at assert blink
        if (mode == SET_TIME_MODE)
        {
            if( alert_blink == HW01_ALERT_BLINK)
            {
                hw01_display_time(clock_minutes, clock_seconds, HW01_LCD_TIME_COLOR); 
            }
            else
            {
                hw01_display_time(clock_minutes, clock_seconds, LCD_COLOR_BLACK); 
            }
        }
        //else if in set alarmt mode blink alartm at assert blink
        else
        {
            if( alert_blink == HW01_ALERT_BLINK)
            {
                hw01_display_alarm(alarm_minutes, alarm_seconds, HW01_LCD_ALARM_COLOR); 
            }
            else
            {
                hw01_display_alarm(alarm_minutes, alarm_seconds, LCD_COLOR_BLACK); 
            }
        }

        //if sw3 asserst gt2s, then print out clock and alarm and return
        if ( alert_sw3 == HW01_ALERT_BUTTON_GT_2S )
        {
            hw01_display_time(clock_minutes, clock_seconds, HW01_LCD_TIME_COLOR); 
            hw01_display_alarm(alarm_minutes, alarm_seconds, HW01_LCD_ALARM_COLOR); 
            return;
        }
    }
}

/**
 * @brief 
 * In run mode, continuously increment clock, and use buzzer 
 */
void run_mode()
{
    //flag to indiciate if alarm is on
    static int alarm = 0;
    int color = HW01_LCD_TIME_COLOR;

    //Continuously loop while in run mode
    while(1)
    {

        //if clock and alarm are equal, and alarm is on, start buzzer, set color to red, and display red clock
        if ( (clock_minutes == alarm_minutes) && (alarm_seconds == clock_seconds) && alarm)
        {
            pwm_buzzer_start();
            color = HW01_LCD_ALARM_ACTIVE_COLOR;
            hw01_display_time(clock_minutes, clock_seconds, color); 
            //set sw1 alert to indicate it should start watching for sw1 press
            alert_sw1 = HW01_ALERT_BLINK;
        }

        //if sw1 is asserted gt2, stop buzzer, erase bell, set alarm to off, change color and print time
        if (alert_sw1 == HW01_ALERT_BUTTON_GT_2S)
        {
            pwm_buzzer_stop();
            hw01_erase_bell();
            alarm = 0;
            alert_sw1 = HW01_ALERT_NONE;
            color = HW01_LCD_TIME_COLOR;
            hw01_display_time(clock_minutes, clock_seconds, HW01_LCD_TIME_COLOR); 
        }
        //if update, then increment and print clock 
        if (alert_update== HW01_ALERT_TIME_UPDATE)
        {
            edit_clock(clock_minutes, clock_seconds, 1, 0, RUN_MODE);
            hw01_display_time(clock_minutes, clock_seconds, color); 
            alert_update = HW01_ALERT_NONE;
        }
        
        //if sw3 is pressed lt2, toggle alarm and draw or erase bell
        if (alert_sw3 == HW01_ALERT_BUTTON_LT_2S)
        {
            if(alarm)
            {
                hw01_erase_bell();
                alarm = 0;
            }
            else
            {
                hw01_draw_bell();
                alarm = 1;
            }
   
            alert_sw3 = HW01_ALERT_NONE;
        }

        //if sw3 asserts gt2, then return
        if ( alert_sw3 == HW01_ALERT_BUTTON_GT_2S )
        {
            return;
        }

    }
   
}
/**
 * @brief
 * Implements the main application for HW01
 */
void hw01_main_app(void)
{
   
    hw01_mode program_mode = SET_TIME_MODE;

    //on reset display alarm and clock to 0
    hw01_display_time(0, 0, HW01_LCD_TIME_COLOR);
    hw01_display_alarm(0, 0, HW01_LCD_ALARM_COLOR);

    while (1)
    {
        //If in run mode call run mode
        if( program_mode == RUN_MODE)
        {
            run_mode();
        }
        //else call set mode in current program mode
        else
        {
            set_mode(program_mode);
        }

        //After exits above call, switch to new mode and deassert mode
        if ( program_mode == RUN_MODE)
        {
            program_mode = SET_ALARM_MODE;
        }
        else
        {
            program_mode = RUN_MODE;
        }

        alert_sw3 = HW01_ALERT_NONE;
    }
}
