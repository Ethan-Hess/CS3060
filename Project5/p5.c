// Multithreaded Producer-Consumer Program
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_BUFFER 10

// Buffer between main and producer
int main_prod_buffer[MAX_BUFFER];
int main_prod_count = 0, main_prod_head = 0, main_prod_tail = 0;
pthread_mutex_t main_prod_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t main_prod_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t main_prod_not_full = PTHREAD_COND_INITIALIZER;

// Buffer between producer and consumer
int *prod_cons_buffer[MAX_BUFFER];
int prod_cons_count = 0, prod_cons_head = 0, prod_cons_tail = 0;
pthread_mutex_t prod_cons_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t prod_cons_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t prod_cons_not_full = PTHREAD_COND_INITIALIZER;

int done = 0;

// Factorization function
int *factorize(int n, int *size)
{
    int *factors = malloc(sizeof(int) * (n + 2));
    int idx = 0;
    factors[idx++] = n;
    int x = n;
    for (int i = 2; i <= x; ++i)
    {
        while (n % i == 0)
        {
            factors[idx++] = i;
            n /= i;
        }
    }
    factors[idx] = 0; // Sentinel
    *size = idx;
    return factors;
}

void *producer(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&main_prod_mutex);
        while (main_prod_count == 0 && !done)
        {
            pthread_cond_wait(&main_prod_not_empty, &main_prod_mutex);
        }
        if (main_prod_count == 0 && done)
        {
            pthread_mutex_unlock(&main_prod_mutex);
            break;
        }
        int num = main_prod_buffer[main_prod_head];
        main_prod_head = (main_prod_head + 1) % MAX_BUFFER;
        main_prod_count--;
        pthread_cond_signal(&main_prod_not_full);
        pthread_mutex_unlock(&main_prod_mutex);

        int fsize;
        int *factors = factorize(num, &fsize);

        pthread_mutex_lock(&prod_cons_mutex);
        while (prod_cons_count == MAX_BUFFER)
        {
            pthread_cond_wait(&prod_cons_not_full, &prod_cons_mutex);
        }
        prod_cons_buffer[prod_cons_tail] = factors;
        prod_cons_tail = (prod_cons_tail + 1) % MAX_BUFFER;
        prod_cons_count++;
        pthread_cond_signal(&prod_cons_not_empty);
        pthread_mutex_unlock(&prod_cons_mutex);
    }

    // Signal consumer that producer is finished
    pthread_mutex_lock(&prod_cons_mutex);
    pthread_cond_signal(&prod_cons_not_empty);
    pthread_mutex_unlock(&prod_cons_mutex);

    return NULL;
}

void *consumer(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&prod_cons_mutex);
        while (prod_cons_count == 0 && !done)
        {
            pthread_cond_wait(&prod_cons_not_empty, &prod_cons_mutex);
        }
        if (prod_cons_count == 0 && done)
        {
            pthread_mutex_unlock(&prod_cons_mutex);
            break;
        }
        int *factors = prod_cons_buffer[prod_cons_head];
        prod_cons_head = (prod_cons_head + 1) % MAX_BUFFER;
        prod_cons_count--;
        pthread_cond_signal(&prod_cons_not_full);
        pthread_mutex_unlock(&prod_cons_mutex);

        int num = factors[0];
        printf("%d: ", num);
        for (int i = 1; factors[i] != 0 && i < num + 2; ++i)
        {
            printf("%d ", factors[i]);
        }
        printf("\n");
        free(factors);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t prod_tid, cons_tid;
    pthread_create(&prod_tid, NULL, producer, NULL);
    pthread_create(&cons_tid, NULL, consumer, NULL);

    for (int i = 1; i < argc; ++i)
    {
        int num = atoi(argv[i]);
        pthread_mutex_lock(&main_prod_mutex);
        while (main_prod_count == MAX_BUFFER)
        {
            pthread_cond_wait(&main_prod_not_full, &main_prod_mutex);
        }
        main_prod_buffer[main_prod_tail] = num;
        main_prod_tail = (main_prod_tail + 1) % MAX_BUFFER;
        main_prod_count++;
        pthread_cond_signal(&main_prod_not_empty);
        pthread_mutex_unlock(&main_prod_mutex);
    }

    pthread_mutex_lock(&main_prod_mutex);
    done = 1;
    pthread_cond_broadcast(&main_prod_not_empty);
    pthread_mutex_unlock(&main_prod_mutex);

    pthread_join(prod_tid, NULL);
    pthread_join(cons_tid, NULL);
    return 0;
}
