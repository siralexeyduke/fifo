/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
   
//-- unity: unit test framework
#include "unity.h"

#include <stdio.h>
#include "string.h"
#include "stdbool.h"
#include "stdint.h"

//-- module being tested
#include "fifo.h" 
      
/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/
#define FIFO_SIZE 16 
#define FIFO_MAX (FIFO_SIZE*2)

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/

typedef struct custom_struct{
    uint8_t  byte0;
    uint32_t word;
    float    fvalue;
    uint8_t  byte1;
} custom_struct_t;

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/
static fifo_t test_fifo;
static custom_struct_t test_fifo_buf[FIFO_MAX];
static custom_struct_t ref_fifo_buf[FIFO_MAX];
static custom_struct_t write_fifo_buf[FIFO_MAX];
static custom_struct_t read_fifo_buf[FIFO_MAX];
	        
/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}	   
		    
/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/
		       
void setUp(void)
{
    memset(test_fifo_buf, 0xFF, FIFO_MAX*sizeof(custom_struct_t));
    memset(ref_fifo_buf, 0xFF, FIFO_MAX*sizeof(custom_struct_t));
    memset(write_fifo_buf, 0xFF, FIFO_MAX*sizeof(custom_struct_t));
    memset(read_fifo_buf, 0xFF, FIFO_MAX*sizeof(custom_struct_t));
    fifo_init(&test_fifo, test_fifo_buf, FIFO_SIZE, sizeof(custom_struct_t));
}
	        
void tearDown(void)
{
}
		 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/

/* Common tests */

/*Test initialized FIFO. It have to be empty and return FIFO_EMPTY on read.*/
void test_fifo_init(void){
    custom_struct_t element;
    fifo_result_t res = fifo_read(&test_fifo, &element, 1);
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT(FIFO_EMPTY, res);
    // Check occupied size of the FIFO
    TEST_ASSERT_EQUAL_UINT( 0, fifo_check(&test_fifo) );
}

/* Write 0 elements to the FIFO */
void test_fifo_write_zero_elem(void){
    custom_struct_t element;
    fifo_result_t res = fifo_write(&test_fifo, &element, 0);
    // Check the result of operation
    TEST_ASSERT_EQUAL_INT(FIFO_ZERO_SIZE, res);
    // And check, that nothing is written
    TEST_ASSERT_EQUAL_MEMORY(ref_fifo_buf, test_fifo_buf, 
                             FIFO_MAX*sizeof(custom_struct_t));
    // Check occupied size of the FIFO
    TEST_ASSERT_EQUAL_UINT( 0, fifo_check(&test_fifo) );
}

/* Read 0 elements from the FIFO */
void test_fifo_read_zero_elem(void){
    custom_struct_t element;
    fifo_result_t res = fifo_read(&test_fifo, &element, 0);
    // Check result of operation
    TEST_ASSERT_EQUAL_INT(FIFO_EMPTY, res);
    // Check occupied size of the FIFO
    TEST_ASSERT_EQUAL_UINT( 0, fifo_check(&test_fifo) );
}

/* Read/Write 1 element */
void test_fifo_rw_one_elem(void){
    custom_struct_t element = {.byte0 = 0xEC, .word = 0xDEADBEEF,
                               .fvalue = 0.1234, .byte1 = 0xCE};
    ref_fifo_buf[0] = element;
    fifo_result_t res = fifo_write( &test_fifo, &element, 1 );
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_OK, res );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, test_fifo_buf, 
                       FIFO_MAX*sizeof(custom_struct_t));

    res = fifo_read( &test_fifo, &element, 1 );
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_EMPTY, res );
    // Check read data
    TEST_ASSERT_EQUAL_MEMORY( &ref_fifo_buf[0], &element, sizeof(custom_struct_t));
}

/* Read/Write Max elements */
void test_fifo_rw_max_elem(void){
    int i;
    for( i = 0; i < FIFO_SIZE; i++ ){
        ref_fifo_buf[i].byte0 = 0xEC;
        ref_fifo_buf[i].word = i;
        ref_fifo_buf[i].fvalue = 0.1234;
        ref_fifo_buf[i].byte1 = 0xCE;
    }

    fifo_result_t res = fifo_write( &test_fifo, ref_fifo_buf, FIFO_SIZE );
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_FULL, res );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, test_fifo_buf, 
                       FIFO_MAX*sizeof(custom_struct_t));

    res = fifo_read( &test_fifo, read_fifo_buf, FIFO_SIZE );
    // Check result of operation
    TEST_ASSERT_EQUAL_INT(FIFO_EMPTY, res);
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, read_fifo_buf, 
                       FIFO_MAX*sizeof(custom_struct_t));
}

/* Read/Write Max+1 elements */
void test_fifo_rw_max_plus_one_elem(void){
    int i;
    for( i = 0; i < FIFO_SIZE + 1; i++ ){
        write_fifo_buf[i].byte0 = 0xEC;
        write_fifo_buf[i].word = i;
        write_fifo_buf[i].fvalue = 0.1234;
        write_fifo_buf[i].byte1 = 0xCE;
    }

    fifo_result_t res = fifo_write( &test_fifo, write_fifo_buf, FIFO_SIZE + 1 );
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_NOT_ENOUGH_SPACE, res );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, test_fifo_buf, 
                       FIFO_MAX*sizeof(custom_struct_t));

    fifo_write( &test_fifo, write_fifo_buf, FIFO_SIZE );
    res = fifo_read( &test_fifo, read_fifo_buf, FIFO_SIZE + 1 );
    // Check result of operation
    TEST_ASSERT_EQUAL_INT( FIFO_NOT_ENOUGH_SPACE, res );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, read_fifo_buf, 
                       FIFO_MAX*sizeof(custom_struct_t));
}

/* Consequent writes and reads */
void test_fifo_consequent_rw(void){
    int i;
    for( i = 0; i < FIFO_SIZE; i++ ){
        write_fifo_buf[i].byte0 = 0xEC;
        write_fifo_buf[i].word = i;
        write_fifo_buf[i].fvalue = 0.1234;
        write_fifo_buf[i].byte1 = 0xCE;
    }
  
    ref_fifo_buf[0] = write_fifo_buf[0];
    
    fifo_result_t res = fifo_write( &test_fifo, &write_fifo_buf[0], 1 );
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_OK, res );
    // Check occupied size of the FIFO
    TEST_ASSERT_EQUAL_UINT( 1, fifo_check(&test_fifo) );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, test_fifo_buf, 
                       FIFO_MAX*sizeof(custom_struct_t));

    ref_fifo_buf[1] = write_fifo_buf[1];

    res = fifo_write( &test_fifo, &write_fifo_buf[1], 1 );
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_OK, res );
    // Check occupied size of the FIFO
    TEST_ASSERT_EQUAL_UINT( 2, fifo_check(&test_fifo) );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, test_fifo_buf, 
                       FIFO_MAX*sizeof(custom_struct_t));

    res = fifo_read( &test_fifo, &read_fifo_buf[0], 1 );
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_OK, res );
    // Check occupied size of the FIFO
    TEST_ASSERT_EQUAL_UINT( 1, fifo_check(&test_fifo) );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, read_fifo_buf, 
                              sizeof(custom_struct_t));

    res = fifo_read( &test_fifo, &read_fifo_buf[1], 1 );
    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_EMPTY, res );
    // Check occupied size of the FIFO
    TEST_ASSERT_EQUAL_UINT( 0, fifo_check(&test_fifo) );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, read_fifo_buf, 
                              2*sizeof(custom_struct_t));
}

/* Wrap around test */
void test_fifo_wraparound(void){
    int i;
    for( i = 0; i < 2*FIFO_SIZE - 5; i++ ){
        write_fifo_buf[i].byte0 = 0xEC;
        write_fifo_buf[i].word = i;
        write_fifo_buf[i].fvalue = 0.1234;
        write_fifo_buf[i].byte1 = 0xCE;
        if( i < FIFO_SIZE ){
            ref_fifo_buf[i] = write_fifo_buf[i];
        } else {
            ref_fifo_buf[i - FIFO_SIZE] = write_fifo_buf[i];
        }
    }

    fifo_result_t res = fifo_write( &test_fifo, write_fifo_buf, FIFO_SIZE - 5 );
    res = fifo_read( &test_fifo, read_fifo_buf, FIFO_SIZE - 5 );
    res = fifo_write( &test_fifo, write_fifo_buf + (FIFO_SIZE-5), FIFO_SIZE);

    // Check result of the operation
    TEST_ASSERT_EQUAL_INT( FIFO_FULL, res );
    // Check buffer contents
    TEST_ASSERT_EQUAL_MEMORY( ref_fifo_buf, test_fifo_buf, 
                       FIFO_MAX*sizeof(custom_struct_t));
}
