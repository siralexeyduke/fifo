#ifndef _FIFO_H_
#define _FIFO_H_

/*
 * Includes and module dependencies
 */

#include <stdbool.h>
#include <stdint.h>

/*
 * Type definitions
 */

/** @todo Hide structure implementation */
typedef struct {
     void * buf;
     unsigned int elem_size;
     unsigned int head;
     unsigned int tail;
     unsigned int size;
     bool empty;
     bool full;
} fifo_t;

typedef enum fifo_result_enum{
	FIFO_OK,
	FIFO_EMPTY,
	FIFO_FULL,
    FIFO_ZERO_SIZE,
    FIFO_NOT_ENOUGH_SPACE
} fifo_result_t;

/*
 * Public functions
 */

/**
 * @brief  Initializes the FIFO structure with the given buffer, FIFO size and
 *         element size
 * @param  f : fifo_t structure 
 * @param  buf : buffer for the FIFO, the size of the buffer must be size*elem_size
 * @param  size : size of the FIFO in elements count 
 * @param  elem_size : size of the FIFO's element, in bytes 
 * @retval None 
 */
void fifo_init(fifo_t * f, void * buf, unsigned int size, unsigned int elem_size);

/**
 * @brief  Reads n elements from the FIFO.
 * @param  f : fifo_t structure 
 * @param  buf : buffer for storing read data 
 * @param  n : number of elements to read 
 * @retval FIFO_EMPTY when FIFO is empty and nothing can be read or when last
 *                    elements were read.
 *         FIFO_ZERO_SIZE when n == 0 provided and no actual read performed.
 *         FIFO_NOT_ENOUGH_SPACE when n is bigger than FIFO's occupied size.
 *         FIFO_OK otherwise.
 */
fifo_result_t fifo_read(fifo_t * f, void * buf, unsigned int n);
 
/**
 * @brief  Writes n elements to the FIFO
 * @param  f : fifo_t structure 
 * @param  buf : buffer containing data to write 
 * @param  n : number of elements to write  
 * @retval FIFO_FULL when FIFO is full and nothing can be write or when last
 *                   elements were written and FIFO became full.
 *         FIFO_ZERO_SIZE when n == 0 provided and no actual write performed.
 *         FIFO_NOT_ENOUGH_SPACE when n is bigger than FIFO's free space.
 *         FIFO_OK otherwise.
 */
fifo_result_t fifo_write(fifo_t * f, const void * buf, unsigned int n);

/**
 * @brief  Checks how many elements are in the FIFO
 * @param  f : fifo_t structure 
 * @retval Number of elements in FIFO 
 */
unsigned int fifo_check(fifo_t * f);

#endif // _FIFO_H_
