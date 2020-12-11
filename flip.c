/*
 * Operating Systems <2INCO> Practical Assignment
 * Threaded Application
 *
 * Niklas Stylianou (1284037)
 * Omer Yildiz (1644300)
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
#include <time.h>

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

// toggle bit at position n
#define BIT_TOGGLE(v,n)     ((v) =  (v) ^ BITMASK(n))

// size of global buffer
#define BUFFER_SIZE         (NROF_PIECES/128) + 1


/*
 * global variable determining the number of currently
 * running worker threads.
 */
static int nrof_workers = 0;


/*
 * data necessary for worker thread computation.
 * Holds references to mutex and condition variable.
 */
struct args_t {
    int i;
    pthread_cond_t *cv;
    pthread_mutex_t *mu;
};


/*
 * Toggle a single bit of buf at position bit_pos.
 */
int toggle_bit(uint128_t buf[], size_t bit_pos)
{
    // we are accessing out of bounds
    if (bit_pos > NROF_PIECES) return -1;

    size_t index = bit_pos / 128;
    char pos = bit_pos % 128;
    BIT_TOGGLE(buf[index], pos);

    return 0;
}


/*
 * worker thread callback. Solve for single number.
 */
static void *solve(void *args_p)
{
    struct args_t *args = (struct args_t*) args_p;

    // lock by mutex
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


/*
 * Prints the result of the computation to stdout. A printed number
 * indicates a bit which is set to 1. A missing number indicates a bit
 * set to 0.
 */
int display_results()
{
    for (int i = 0; i < NROF_PIECES; i++)
    {
        size_t index = i / 128;
        char pos = i % 128;

        // Only print if the bit is set
        if (BIT_IS_SET(buffer[index], pos))
            printf("%d\n", i + 1);
    }

    return 0;
}


int main (void)
{
    #ifdef BENCHMARK
        // start clock for benchmarking reasons
        clock_t start = clock();
    #endif

    // Set all bits to 1 (i.e. white)
    memset(buffer, ~0, sizeof(buffer));

    // TODO: check for failed calls
    int ret;

    // initialize condition variable and mutex
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

    for (int i = 2; i <= NROF_PIECES; i++)
    {
        // block main thread as long as max. nr of workers are running
        pthread_mutex_lock(&mu);
        while (nrof_workers >= NROF_THREADS)
        {
            pthread_cond_wait(&cv, &mu);
        }

        // Initialize necessary data for creating thread
        struct args_t *args = malloc(sizeof(struct args_t));
        args->i = i;
        args->cv = &cv;
        args->mu = &mu;

        pthread_t tid;
        pthread_attr_t tattr;

        // set thread into detached state to auto free recourses after return
        ret = pthread_attr_init(&tattr);
        ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

        // increment nr of workers and unlock
        nrof_workers++;
        pthread_mutex_unlock(&mu);

        // create a worker
        ret = pthread_create(&tid, &tattr, solve, args);
    }

    // wait for condition variable to be set
    pthread_mutex_lock(&mu);
    while (nrof_workers > 0)
    {
        ret = pthread_cond_wait(&cv, &mu);
    }
    pthread_mutex_unlock(&mu);

    // destroy mutex
    pthread_mutex_destroy(&mu);
    pthread_cond_destroy(&cv);

    display_results();

    #ifdef BENCHMARK
        clock_t end = clock();
        double total_cpu_time = ((double) end - start) / CLOCKS_PER_SEC;
        printf("\n\n>>> %fs\n\n", total_cpu_time);
    #endif

    return 0;
}


/*
 * Old implementation. Single threaded.
 */
int solve_single_threaded()
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
