#include "fifo.h"

#include <string.h> // For memcpy()

/**
 * @brief  Initializes the FIFO structure with the given buffer, FIFO size and
 *         element size
 * @param  f : fifo_t structure 
 * @param  buf : buffer for the FIFO, the size of the buffer must be size*elem_size
 * @param  size : size of the FIFO in elements count 
 * @param  elem_size : size of the FIFO's element, in bytes 
 * @retval None 
 */
void fifo_init(fifo_t * f, void * buf, unsigned int size, unsigned int elem_size){
     f->head = 0;
     f->tail = 0;
     f->empty = true;
     f->full = false;
     f->size = size;
     f->elem_size = elem_size;
     f->buf = buf;
}

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
fifo_result_t fifo_read(fifo_t * f, void * buf, unsigned int n){
    if( f->empty ){
        return FIFO_EMPTY;
    } else if( n == 0 ){
        return FIFO_ZERO_SIZE;
    } else if( n > fifo_check( f ) ){
        return FIFO_NOT_ENOUGH_SPACE;
    } else {
        f->full = false;
        // Handle wraparound
        unsigned int buf_offset = f->tail * f->elem_size;
        unsigned int n_in_bytes = n * f->elem_size;
        if( f->tail + n > f->size ){
            unsigned int bef_end = (f->size - f->tail) * f->elem_size;
            memcpy( buf, f->buf + buf_offset, bef_end );
            memcpy( buf + bef_end, f->buf, n_in_bytes - bef_end );
            f->tail = n + f->tail - f->size;
        } else {
            memcpy( buf, f->buf + buf_offset, n_in_bytes );
            f->tail += n;
            if(f->tail == f->size){
                f->tail = 0;
            }
        }
        // Check whether FIFO is empty
        if( f->tail == f-> head ){
            f->empty = true;
            return FIFO_EMPTY;
        }
        return FIFO_OK;
    }
}

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
fifo_result_t fifo_write(fifo_t * f, const void * buf, unsigned int n){
    if( f->full ){
        return FIFO_FULL;
    } else if( n == 0 ){
        return FIFO_ZERO_SIZE;
    } else if( n > f->size - fifo_check( f ) ){
        return FIFO_NOT_ENOUGH_SPACE;
    } else {
        // Reset empty flag
        f->empty = false;
        // Handle wraparound
        unsigned int buf_offset = f->head * f->elem_size;
        unsigned int n_in_bytes = n * f->elem_size;
        if( f->head + n > f->size ){
            unsigned int bef_end = (f->size - f->head) * f->elem_size;
            memcpy( f->buf + buf_offset, buf, bef_end );
            memcpy( f->buf, buf + bef_end, n_in_bytes - bef_end );
            f->head = n + f->head - f->size;
        } else {
            memcpy( f->buf + buf_offset, buf, n_in_bytes );
            f->head += n;
            if(f->head == f->size){
                f->head = 0;
            }
        }
        // Check whether FIFO is full
        if( f->head == f->tail ){
            f->full = true;
            return FIFO_FULL;
        }
        return FIFO_OK;
    }
}

/**
 * @brief  Checks how many elements are in the FIFO
 * @param  f : fifo_t structure 
 * @retval Number of elements in FIFO 
 */
unsigned int fifo_check(fifo_t * f){
    if( f->empty ){
        return 0;
    } else if ( f->full ){
        return f->size;
    } else if(f->head > f->tail){
		return f->head - f->tail;
	} else {
		return f->size - f->tail + f->head;
	}
}

