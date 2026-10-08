/* Purpose:
 *  Implement a task queue using pthread.
 * 
 * Author:
 *  AkashiSensei
 * 
 * Compile:
 *  gcc -o taskqueue taskqueue.c -lpthread
 * 
 * Usage:
 *  ./taskqueue  <number of threads>  <number of tasks> 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "time.h"

#define RANDOM_MAX 20
#define TASK_QUEUE_MAX 100
#define DEBUG 1

#define TASK_INSERT 0
#define TASK_DELETE 1
#define TASK_QUERY 2
#define TASK_LIST 3
#define TASK_TYPES 4

/* random */
int gen_task_type();
int gen_task_operand();

/* task queue */
typedef struct task_node{
    int num;
    int type;
    int operand;
}task;
pthread_mutex_t TASKQ_MUTEX;
task TASK_QUEUE[TASK_QUEUE_MAX];
int TASKQ_HEAD;
int TASKQ_REAR;
int HAVE_WORK;
int is_taskq_empty();
int is_taskq_full();
void taskq_push(int num, int type, int operand);
task taskq_pop();

/* condition variables */
pthread_mutex_t COND_MUTEX;
pthread_cond_t HAVE_TASK_COND;

/* link list */
pthread_mutex_t LIST_MUTEX;
typedef struct link_node{
    int num;
    struct link_node * next;
}node;
node * LINK_LIST;
int insert_node(int num);
int delete_node(int num);
int query_node(int num);
void traverse_list();

/* worker threads routine */
void * worker(void * rank);


int main(int argc, char * argv[]) {
    int thread_cnt;
    int task_cnt;
    pthread_t * threads;

    /* get arguments */
    if (argc != 3) {
        return -1;
    }
    thread_cnt = atoi(argv[1]);
    task_cnt = atoi(argv[2]);

    /* init random */
    srand(time(NULL));

    /* init task queue */
    TASKQ_REAR = 0;
    TASKQ_HEAD = 0;
    pthread_mutex_init(&TASKQ_MUTEX, NULL);

    /* init condition variable */
    pthread_mutex_init(&COND_MUTEX, NULL);
    pthread_cond_init(&HAVE_TASK_COND, NULL);

    /* init link list */
    pthread_mutex_init(&LIST_MUTEX, NULL);

    /* create threads */
    HAVE_WORK = 1;
    threads = (pthread_t *) malloc(thread_cnt * sizeof(pthread_t));
    for (long i = 0; i < thread_cnt; i++) {
        pthread_create(threads + i, NULL, worker, (void *) i);
    }

    /* create tasks */
    for (int i = 0; i < task_cnt; i++) {
        pthread_mutex_lock(&TASKQ_MUTEX);
        taskq_push(i, gen_task_type(), gen_task_operand());

        pthread_cond_signal(&HAVE_TASK_COND);
        pthread_mutex_unlock(&TASKQ_MUTEX);

        #ifdef DEBUG
        printf("Main: create task %d and send a signal\n", i);
        #endif
    }

    /* scatter tasks */
    pthread_mutex_lock(&TASKQ_MUTEX);
    while (!is_taskq_empty()) {
        pthread_mutex_unlock(&TASKQ_MUTEX);

        pthread_cond_signal(&HAVE_TASK_COND);
        #ifdef DEBUG
        printf("Main: send a signal\n");
        #endif

        pthread_mutex_lock(&TASKQ_MUTEX);
    }
    HAVE_WORK = 0;
    pthread_mutex_unlock(&TASKQ_MUTEX);
    
    /* signal all for end */
    pthread_cond_broadcast(&HAVE_TASK_COND);
    #ifdef DEBUG
    printf("Main: signal all\n");
    #endif

    /* join threads*/
    for (long i = 0; i < thread_cnt; i++) {
        pthread_cond_broadcast(&HAVE_TASK_COND);
        pthread_join(threads[i], NULL);
    }

    /* task queue finished and print result */
    printf("Main: all tasks finished, ");
    traverse_list();

    /* destroy */
    pthread_mutex_destroy(&LIST_MUTEX);
    pthread_cond_destroy(&HAVE_TASK_COND);
    pthread_mutex_destroy(&COND_MUTEX);
    pthread_mutex_destroy(&TASKQ_MUTEX);
    free(threads);

    return 0;
}

int gen_task_operand() {
    return rand() % RANDOM_MAX; 
}

int gen_task_type() {
    return rand() % TASK_TYPES;
}

int is_taskq_empty() {
    return TASKQ_HEAD == TASKQ_REAR;
}

int is_taskq_full() {
    return (TASKQ_REAR + 1) % TASK_QUEUE_MAX == TASKQ_HEAD;
}

void taskq_push(int num, int type, int operand) {
    if (is_taskq_full()) {
        return;
    }

    TASK_QUEUE[TASKQ_REAR].num = num;
    TASK_QUEUE[TASKQ_REAR].type = type;
    TASK_QUEUE[TASKQ_REAR].operand = operand;

    TASKQ_REAR++;
    TASKQ_REAR %= TASK_QUEUE_MAX;
}

task taskq_pop() {
    task ret = {-1, -1, 0};
    if (is_taskq_empty()) {
        return ret;
    }

    ret = TASK_QUEUE[TASKQ_HEAD++];
    TASKQ_HEAD %= TASK_QUEUE_MAX;
    return ret;
}

void * worker(void * rank) {
    long my_rank = (long) rank;
    task cur_task;
    int status;

    while (HAVE_WORK) {
        pthread_mutex_unlock(&TASKQ_MUTEX);

        /* wait for having task */
        pthread_mutex_lock(&COND_MUTEX);
        #ifdef DEBUG
        printf("Thread %ld: wait\n", my_rank);
        #endif
        while (pthread_cond_wait(&HAVE_TASK_COND, &COND_MUTEX) != 0);
        #ifdef DEBUG
        printf("Thread %ld: wake up\n", my_rank);
        #endif
        pthread_mutex_unlock(&COND_MUTEX);

        /* get task */
        pthread_mutex_lock(&TASKQ_MUTEX);
        if (!HAVE_WORK) {
            break;
        }
        cur_task = taskq_pop();
        pthread_mutex_unlock(&TASKQ_MUTEX);
        
        /* work */
        printf("Thread %ld: task %d: ", my_rank, cur_task.num);
        switch (cur_task.type) {
            case TASK_INSERT:
                status = insert_node(cur_task.operand);
                if (status == 0) printf("%d is inserted\n", cur_task.operand);
                else printf("%d cannot be inserted\n", cur_task.operand);
                break;
            case TASK_DELETE:
                status = delete_node(cur_task.operand);
                if (status == 0) printf("%d is deleted\n", cur_task.operand);
                else printf("%d cannot be deleted\n", cur_task.operand);
                break;
            case TASK_QUERY:
                status = query_node(cur_task.operand);
                if (status == 0) printf("%d is in the list\n", cur_task.operand);
                else printf("%d is not in the list\n", cur_task.operand);
                break;
            default:
                traverse_list();
        }

        pthread_mutex_lock(&TASKQ_MUTEX);

        #ifdef DEBUG
        printf("Thread %ld: end a loop, have %swork\n", my_rank, HAVE_WORK ? "" : "no ");
        #endif
    }
    pthread_mutex_unlock(&TASKQ_MUTEX);

    #ifdef DEBUG
    printf("Thread %ld: finish\n", my_rank);
    #endif
}

int insert_node(int num) {
    pthread_mutex_lock(&LIST_MUTEX);

    node **current = &LINK_LIST;
    while (*current != NULL && (*current)->num < num) {
        current = &(*current)->next;
    }

    if (*current != NULL && (*current)->num == num) {
        pthread_mutex_unlock(&LIST_MUTEX);
        return -1;
    }

    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node) {
        pthread_mutex_unlock(&LIST_MUTEX);
        return -2;
    }

    new_node->num = num;
    new_node->next = *current;
    *current = new_node;

    pthread_mutex_unlock(&LIST_MUTEX);
    return 0;
}

int delete_node(int num) {
    pthread_mutex_lock(&LIST_MUTEX);

    node **current = &LINK_LIST;
    while (*current != NULL && (*current)->num < num) {
        current = &(*current)->next;
    }

    if (*current == NULL || (*current)->num != num) {
        pthread_mutex_unlock(&LIST_MUTEX);
        return -1;
    }

    node *temp = *current;
    *current = (*current)->next;
    free(temp);

    pthread_mutex_unlock(&LIST_MUTEX);
    return 0; 
}

int query_node(int num) {
    pthread_mutex_lock(&LIST_MUTEX);

    node *current = LINK_LIST;
    while (current != NULL) {
        if (current->num == num) {
            pthread_mutex_unlock(&LIST_MUTEX);
            return 0;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&LIST_MUTEX);
    return -1;
}

void traverse_list() {
    pthread_mutex_lock(&LIST_MUTEX);

    node *current = LINK_LIST;
    printf("Print List: ");
    while (current != NULL) {
        printf("%d ", current->num);
        current = current->next;
    }
    printf("\n");

    pthread_mutex_unlock(&LIST_MUTEX);
}