/**
 * @file remote_uart_rx.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "remote_uart.h"
/*******************************************************************************
 * Macros
 ******************************************************************************/

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
extern cyhal_uart_t remote_uart_obj;

Circular_Buffer *Rx_Circular_Buffer;
volatile bool ALERT_UART_RX = false;

/**
 * @brief
 * Return a message from the circular buffer
 * @param msg
 * Starting address to place the message
 * @param max_size
 * Max number of characters to copy
 * @return true
 * Message copied successfully
 * @return false
 * No message was found in circular buffer
 */
bool remote_uart_rx_data_async(char *msg, uint16_t max_size)
{
    /* ADD CODE */
    int count = 0; 
    /* If the msg pointer is NULL, return false*/
    if (msg == NULL){
        return false;
    }
    /* If the circular buffer is empty, return false */
    if (circular_buffer_empty(Rx_Circular_Buffer)){
        return false;
    }

    /* Disable interrupts -- Disable NVIC */
    __disable_irq(); 

    /* Grab characters from the circular buffer until the buffer
     *  is empty OR the maximum number of characters are copied*/
    while(!circular_buffer_empty(Rx_Circular_Buffer) || count <= max_size){
        msg[count] = circular_buffer_remove(Rx_Circular_Buffer);
        count++;
    }

    /* Re-enable interrupts -- Enable NVIC */
    __enable_irq();

    /* Return true */
    return true;
}

/**
 * @brief
 * Enables Rx Interrupts for the remote UART. 
 * of data will utilize circular buffers to help minimize the amount
 * of time spent waiting for the UART interface.
 */
void remote_uart_rx_interrupts_init(void)
{
    /* ADD CODE */
    
    /* Initialize Rx_Circular_Buffer */
    Rx_Circular_Buffer = circular_buffer_init(RX_BUFFER_SIZE);

    /* Turn on Rx interrupts */
    cyhal_uart_enable_event(&remote_uart_obj, CYHAL_UART_IRQ_RX_NOT_EMPTY, 7, true);
}

/**
 * @brief
 * Implements the Rx portion of the UART Handler
 */
void remote_uart_event_handler_process_rx(void)
{
    //cy_rslt_t status;
    uint8_t c;

    /* ADD CODE to receive a character using the HAL UART API */
    cyhal_uart_getc(&remote_uart_obj, &c, 1);
    //c = REMOTE_SCB -> RX_FIFO_RD;
    /* check for the end of a message to set the alert variable*/
    /* Add the character to the circular buffer if not the 
    * of the message
    */
    if (c == '\n' || c == '\r')  {
        ALERT_UART_RX = true; 
    } else {
        circular_buffer_add(Rx_Circular_Buffer, c);
    }  

}