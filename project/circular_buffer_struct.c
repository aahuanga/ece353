/**
 * @file circular_buffer.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "circular_buffer_struct.h"

//*****************************************************************************
// Initializes a circular buffer.
//
// Parameters
//    max_size:   Number of entries in the circular buffer.
//*****************************************************************************
struct Projectile_Queue * p_circular_buffer_init(uint16_t max_size)
{
   /* Allocate memory from the heap fro the circular buffer struct */
   struct Projectile_Queue *buffer = malloc(sizeof(struct Projectile_Queue));

   /* Allocate memory from the heap that will be used to store the characters/data
    * in the circular buffer
    */
   buffer->queue = malloc(sizeof(struct proj) * max_size);

   /* Initialize the max_size, produce count, and consume count*/ 
   buffer->max_size = max_size;
   buffer->produce_count = 0;
   buffer->consume_count = 0;

   /* Return a pointer to the circular buffer data structure */
   return buffer;
}

//*****************************************************************************
// Returns a circular buffer to the heap
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
void p_circular_buffer_delete(struct Projectile_Queue * buffer)
{
   free((void *)buffer->queue);
   free(buffer);
}

//*****************************************************************************
// Returns true if the circular buffer is empty.  Returns false if it is not.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
bool p_circular_buffer_empty(struct Projectile_Queue *buffer)
{
  /* Use the buffer->produce_count and buffer->consume count to determine if 
   * the circular buffer is empty 
   */
  if (buffer-> produce_count == buffer->consume_count){
    return true;
  }
  return false;
}

//*****************************************************************************
// Returns true if the circular buffer is full.  Returns false if it is not.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
bool p_circular_buffer_full(struct Projectile_Queue *buffer)
{
  /* Use the buffer->produce_count and buffer->consume count to determine if 
   * the circular buffer is full
   */
  if ((buffer-> produce_count - buffer->consume_count) == buffer->max_size){
    return true;
  }
  return false;

}
//*****************************************************************************
// Adds a character to the circular buffer.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//    c       :   Character to add.
//*******************************************************************************
bool p_circular_buffer_add(struct Projectile_Queue *buffer, struct proj c)
{
  uint8_t index = buffer->produce_count % buffer->max_size;

  // Use the function defined above to determine if the circular buffer is full
  // If the circular buffer is full, return false.
  if (p_circular_buffer_full(buffer)){
    return false;
  }
  // Add the data to the circular buffer.  
  buffer->queue[index] = c;
  buffer->produce_count += 1;
  // Return true to indicate that the data was added to the
  // circular buffer.
  return true;

}

//*****************************************************************************
// Removes the oldest character from the circular buffer.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//
// Returns
//    The character removed from the circular buffer.  If the circular buffer
//    is empty, the value of the character returned is set to 0.
//*****************************************************************************
struct proj p_circular_buffer_remove(struct Projectile_Queue *buffer)
{
  struct proj return_char;

  uint8_t index = buffer->consume_count % buffer->max_size;

  // If the circular buffer is empty, return 0.
  // Use the function defined above to determine if the circular buffer is empty
  if (p_circular_buffer_empty(buffer)){
    struct proj p= { 0, 0, 0};
    return p;
  }
  // remove the character from the circular buffer
  return_char = buffer->queue[index];
  buffer->consume_count += 1;

  // return the character
  return return_char;

}

