/*
 * Operating Systems <2INCO> Practical Assignment
 * Threaded Application
 *
 * Niklas Stylianou (1284037)
 * STUDENT_NAME_2 (STUDENT_NR_2)
 *
 * Grading: * Students who hand in clean code that fully satisfies the minimum requirements will get an 8.
 * Extra steps can lead to higher marks because we want students to take the initiative.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>          // for perror()
#include <string.h>
#include <pthread.h>

#include "uint128.h"
#include "flip.h"

// create a bitmask where bit at position n is set
#define BITMASK(n)          (((uint128_t) 1) << (n))

// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))

// set bit n in v
#define BIT_SET(v,n)        ((v) =  (v) |  BITMASK(n))

// clear bit n in v
#define BIT_CLEAR(v,n)      ((v) =  (v) & ~BITMASK(n))

#define BIT_TOGGLE(v,n)     ((v) =  (v) ^ BITMASK(n))

#define BUFFER_SIZE         (NROF_PIECES/128) + 1


int toggle_bit(uint128_t buf[], size_t bit_pos)
{
    // we are accessing out of bounds
    if (bit_pos > NROF_PIECES) return -1;

    size_t index = bit_pos / 128;
    char pos = bit_pos % 128;
    BIT_TOGGLE(buf[index], pos);

    return 0;
}


int main (void)
{
    // TODO: start threads to flip the pieces and output the results
    // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)

    // Set all bits to 1 (i.e. white)
    memset(buffer, ~0, sizeof(buffer));

    for(int i = 1; i <= NROF_PIECES; i++)
    {
        for(int j = i; j <= NROF_PIECES; j += i)
        {
            int code = toggle_bit(buffer, j - 1);
            if (code == -1) {
                printf("Buffer access out of bounds.\n");
                exit(EXIT_FAILURE);
            }
        }
    }


    for (int i = 0; i < BUFFER_SIZE; i++) {
        uint128_t ll = buffer[i];
        printf ("%lx%lx\n", HI(ll), LO(ll));
    }
    printf("\n");


    return (0);
}
