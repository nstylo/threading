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


// TODO: this is currently not mutex locked
static int nrof_workers = 0;


struct args_t {
    int i;
    pthread_cond_t *cv;
    pthread_mutex_t *mu;
};

int toggle_bit(uint128_t buf[], size_t bit_pos)
{
    // we are accessing out of bounds
    if (bit_pos > NROF_PIECES) return -1;

    size_t index = bit_pos / 128;
    char pos = bit_pos % 128;
    BIT_TOGGLE(buf[index], pos);

    return 0;
}

static void *solve(void *args_p)
{
    struct args_t *args = (struct args_t*) args_p;

    // lock by mutex
    // TODO: is this the correct way?
    pthread_mutex_lock(args->mu);

    for(int j = args->i; j <= NROF_PIECES; j += args->i)
    {
        int ret = toggle_bit(buffer, j - 1);
        if (ret == -1) {
            perror("Buffer access out of bounds.\n");
            return (void*)-1;
        }
    }

    // unlock and signal back to main thread
    nrof_workers--;
    pthread_mutex_unlock(args->mu);
    pthread_cond_signal(args->cv);

    // reclaim ressources
    free(args);

    return (void*)0;
}

int display_results()
{
    for (int i = 0; i < NROF_PIECES; i++)
    {
        size_t index = i / 128;
        char pos = i % 128;

        // Only print if the bit is set
        if(BIT_IS_SET(buffer[index], pos))
            printf("%d\n", i + 1);
    }

    return 0;
}

int main (void)
{
    // Set all bits to 1 (i.e. white)
    memset(buffer, ~0, sizeof(buffer));

    int ret;

    // initialize condition variable and mutex
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

    for (int i = 1; i <= NROF_PIECES; i++)
    {
        pthread_mutex_lock(&mu);
        while (nrof_workers >= NROF_THREADS)
        {
            pthread_cond_wait(&cv, &mu);
        }

        struct args_t *args = malloc(sizeof(struct args_t));
        args->i = i;
        args->cv = &cv;
        args->mu = &mu;

        // TODO: make function // <-- may not be needed
        pthread_t tid;
        pthread_attr_t tattr;

        // set thread into detached state to auto free recourses after return
        ret = pthread_attr_init(&tattr);
        ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

        // increment nr of workers and unlock
        nrof_workers++;
        // printf("nr of workers: %d\n", nrof_workers);
        pthread_mutex_unlock(&mu);

        // create a worker
        ret = pthread_create(&tid, &tattr, solve, args);
    }


    // wait for condition variable to be set
    // printf("\nfinal number of workers left: %d\n\n", nrof_workers);
    pthread_mutex_lock(&mu);
    while (nrof_workers > 0)
    {
        ret = pthread_cond_wait(&cv, &mu);
    }
    pthread_mutex_unlock(&mu);

    // print result, note that depending on NROF_PIECES, there might be trailing f's
    // for (int i = 0; i < BUFFER_SIZE; i++) {
    //     uint128_t ll = buffer[i];
    //     printf ("%lx%lx\n", HI(ll), LO(ll));
    // }

    // destroy mutex
    pthread_mutex_destroy(&mu);
    pthread_cond_destroy(&cv);

    display_results();

    return 0;
}


int solve_non_threaded()
{

    for(int i = 1; i <= NROF_PIECES; i++)
    {
        for(int j = i; j <= NROF_PIECES; j += i)
        {
            
            int ret = toggle_bit(buffer, j - 1);
            if (ret == -1) {
                printf("Buffer access out of bounds.\n");
                return -1;
            }
        }
    }

    return 0;
}

