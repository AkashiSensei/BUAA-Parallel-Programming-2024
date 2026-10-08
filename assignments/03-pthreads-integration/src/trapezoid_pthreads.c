/* Purpose:
 *  Calculate definite integrals using pthreads.
 * 
 * Author:
 *  AkashiSensei
 * 
 * Compile:
 *  gcc -o pth_trap pth_trap.c -lpthread
 * 
 * Usage:
 *  ./pth_trap  <number of threads>  <number of method> 
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#define DEBUG 1

/* method constant */
#define BUSY_WAIT 1
#define MUTEX 2
#define SEMAPHORE 3

/* exclusive global variables */
int HEIGHT_SUM;

/* read-only global variables for threads */
double A;
double B;
double STEP;
int N;
int CNT_PTH;
int LOCAL_N;
int METHOD;

/* busy-wait variables */
int BW_FLAG;
/* mutex variables */
pthread_mutex_t S_MUTEX;
/* semaphore variables*/
sem_t S_SEM;

/* functions without using global variables */
double f(double x);

/* exclusive functions */
void * thread_cal(void * rank);
void reduce_busy_wait(long rank, double local_sum);
void reduce_mutex(long rank, double local_sum);
void reduce_semaphore(long rank, double local_sum);

int main(int argc, char * argv[]) {
    /* get arguments */
    if (argc != 3) {
        printf("Usage: %s  <number of threads>  <number of method> \n", argv[0]);
        return -1;
    }

    CNT_PTH = atoi(argv[1]);
    if (CNT_PTH < 1) {
        return -1;
    }

    METHOD = atoi(argv[2]);
    switch (METHOD) {
        case BUSY_WAIT:
            printf("Using method busy-wait\n");
            BW_FLAG = 0;
            break;
        case MUTEX:
            pthread_mutex_init(&S_MUTEX, NULL);
            printf("Using method mutex\n");
            break;
        case SEMAPHORE:
            sem_init(&S_SEM, 0, 1);
            printf("Using method semaphore\n");
            break;
        default:
            printf("Not a legal method\n");
            return -1;
    }


    /* get input data */
    printf("Please input a, b, n:\n");
    scanf("%lf %lf %d", &A, &B, &N);
    if (N % CNT_PTH != 0) {
        printf("N cannot divided by number of threads\n");
        return -1;
    }

    LOCAL_N = N / CNT_PTH;
    STEP = (B - A) / (N - 1);


    /* create threads */
    pthread_t * pth_handles = (pthread_t *) malloc(CNT_PTH * sizeof(pthread_t));
    for (long i = 0; i < CNT_PTH; i++) {
        pthread_create(pth_handles + i, NULL, thread_cal, (void *) i);
    }


    /* wait for child threads end and join them*/
    for (int i = 0; i < CNT_PTH; i++) {
        pthread_join(pth_handles[i], NULL);
    }
    if (METHOD == MUTEX) {
        pthread_mutex_destroy(&S_MUTEX);
    } else if (METHOD == SEMAPHORE) {
        sem_destroy(&S_SEM);
    }


    /* calculate result and output */
    printf("Estimate of the integral: %lf\n", HEIGHT_SUM * STEP);

    return 0;
}

double f(double x) {
    return x * x + x;
}

void * thread_cal(void * rank) {
    long my_rank;
    double local_sum;
    double local_x;

    my_rank = (long) rank;
    local_x = A + (B - A) / CNT_PTH * my_rank;
    local_sum = 0;

    #ifdef DEBUG
    printf("pthread %ld start at %lf\n", my_rank, local_x);
    #endif

    /* 
     * Compared to calculating the x value through multiplication and division 
     * each time, this method loses accuracy but improves efficiency.
    */
    for (int i = 0; i < LOCAL_N; i++) {
        local_sum += f(local_x);
        local_x += STEP;
    }

    #ifdef DEBUG
    printf("pthread %ld end at %lf, get sum %lf\n", my_rank, local_x, local_sum);
    #endif

    switch (METHOD) {
    case BUSY_WAIT:
        reduce_busy_wait(my_rank, local_sum);
        break;
    case MUTEX:
        reduce_mutex(my_rank, local_sum);
        break;
    case SEMAPHORE:
        reduce_semaphore(my_rank, local_sum);
        break;
    default:
        return NULL;
    }

    return NULL;
}

void reduce_busy_wait(long rank, double local_sum) {
    #ifdef DEBUG
    printf("pthread %ld start busy-wait\n", rank);
    #endif

    while (rank != BW_FLAG);

    HEIGHT_SUM += local_sum;

    #ifdef DEBUG
    printf("pthread %ld has already add to sum\n", rank);
    #endif

    BW_FLAG++;
}

void reduce_mutex(long rank, double local_sum) {
    #ifdef DEBUG
    printf("pthread %ld ask for mutex\n", rank);
    #endif

    pthread_mutex_lock(&S_MUTEX);

    HEIGHT_SUM += local_sum;

    #ifdef DEBUG
    printf("pthread %ld has already add to sum\n", rank);
    #endif

    pthread_mutex_unlock(&S_MUTEX);
}

void reduce_semaphore(long rank, double local_sum) {
    #ifdef DEBUG
    printf("pthread %ld wait for semaphore\n", rank);
    #endif

    sem_wait(&S_SEM);

    HEIGHT_SUM += local_sum;

    #ifdef DEBUG
    printf("pthread %ld has already add to sum\n", rank);
    #endif

    sem_post(&S_SEM);
}