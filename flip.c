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

#define BUFFER_SIZE (NROF_PIECES/128) + 1

void toggle(uint128_t buf[], size_t foo)
{
    // int i = 0;
    // for(i = 0; i < NROF_PIECES; i++)
    // {
    //     if( (i % pos) == 0)
    // }

    int i = (foo/128);
    int pos = foo % 128;
    BIT_TOGGLE(buf[i], pos);
    // printf ("Buf[%d] : %lx%lx\n", i, HI(buf[i]), LO(buf[i]));


}


int main (void)
{
    // TODO: start threads to flip the pieces and output the results
    // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)
    memset(buffer, ~0, sizeof(buffer));

    int i, j;


    // toggle(buffer, 0);
    // toggle(buffer, 1);
    // toggle(buffer, 2);
    // toggle(buffer, 3);
    // printf("\n");

    for(i = 1 ; i<= NROF_PIECES; i++)
    {
        for(j = i; j<= NROF_PIECES; j += i)
        {

            toggle(buffer, j-1);
        }
    }
    

    

    // BIT_TOGGLE(buffer[1], 5);

    for (i = 0; i < (NROF_PIECES/128) + 1; i++) {
        uint128_t ll = buffer[i];
        // printf ("all 1's : %lx%lx\n", HI(ll), LO(ll));
        printf ("%lx%lx\n", HI(ll), LO(ll));
    }
    printf("\n");


    return (0);
}
