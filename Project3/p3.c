#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_FACTORS 10
#define MAX_NUMBERS 25

typedef struct
{
    int number;
    int factors[MAX_FACTORS];
    int factor_count;
} FactorResult;

void *factorize(void *arg)
{
    FactorResult *result = (FactorResult *)arg;
    int n = result->number;
    int count = 0;
    int d = 2;
    while (n > 1 && count < MAX_FACTORS)
    {
        while (n % d == 0)
        {
            result->factors[count++] = d;
            n /= d;
            if (count >= MAX_FACTORS)
                break;
        }
        d++;
    }
    result->factor_count = count;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage:%s <number to factor>...\n", argv[0]);
        return 1;
    }
    if (argc - 1 > MAX_NUMBERS)
    {
        fprintf(stderr, "Error: Too many numbers (max %d)\n", MAX_NUMBERS);
        return 1;
    }
    pthread_t threads[MAX_NUMBERS];
    FactorResult results[MAX_NUMBERS];
    int i;
    for (i = 1; i < argc; ++i)
    {
        int num = atoi(argv[i]);
        if (num < 2)
        {
            fprintf(stderr, "Error: Number must be >= 2\n");
            return 1;
        }
        results[i - 1].number = num;
        results[i - 1].factor_count = 0;
        pthread_create(&threads[i - 1], NULL, factorize, &results[i - 1]);
    }
    for (i = 0; i < argc - 1; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    for (i = 0; i < argc - 1; ++i)
    {
        printf("%d:", results[i].number);
        for (int j = 0; j < results[i].factor_count; ++j)
        {
            printf(" %d", results[i].factors[j]);
        }
        printf("\n");
    }
    return 0;
}
