/**
 * @file circular_buffer.h
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __CIRCULAR_BUFFER_STRUCT_H_
#define __CIRCULAR_BUFFER_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "task_LCD.h"

struct Projectile_Queue {
    volatile uint32_t produce_count;
    volatile uint32_t consume_count;
    uint16_t max_size;
    volatile struct proj *queue;
};


//*****************************************************************************
// Initializes a circular buffer.
//
//      1. Initialize the produce_count
//      2. Initialize the consume_count
//      3. Initialize the max_size
//      4. malloc enough memory from the heap for the data array
//
// Parameters
//    buffer  :   The address of the circular buffer.
//    max_size:   Number of entries in the circular buffer.
//*****************************************************************************
struct Projectile_Queue* p_circular_buffer_init(uint16_t max_size);

//*****************************************************************************
// Returns a circular buffer to the heap
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
void p_circular_buffer_delete(struct Projectile_Queue * buffer);

//*****************************************************************************
// Adds a character to the circular buffer.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//    c       :   Character to add.
//*******************************************************************************
bool p_circular_buffer_add(struct Projectile_Queue *buffer, struct proj c );

//*****************************************************************************
// Removes the oldest character from the circular buffer.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
struct proj p_circular_buffer_remove(struct Projectile_Queue  *buffer);

//*****************************************************************************
// Returns true if the circular buffer is empty.  Returns false if it is not.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
bool p_circular_buffer_empty(struct Projectile_Queue *buffer);

//*****************************************************************************
// Returns true if the circular buffer is full.  Returns false if it is not.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
bool p_circular_buffer_full(struct Projectile_Queue *buffer);

#endif /* circular_buffer_H_ */
