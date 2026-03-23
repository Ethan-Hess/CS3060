#include <stdio.h>
#include <stdlib.h>

// Maximum number of block requests (not counting the start position).
#define MAX_REQUESTS 100

// FCFS: service requests in the same order they arrive.
int fcfs_total_seek(const int requests[], int request_count, int start_pos)
{
    int total_seek = 0;
    int current = start_pos;

    for (int i = 0; i < request_count; i++)
    {
        total_seek += abs(current - requests[i]);
        current = requests[i];
    }

    return total_seek;
}

// SSTF: repeatedly choose the pending request closest to the current head.
int sstf_total_seek(const int requests[], int request_count, int start_pos)
{
    int total_seek = 0;
    int current = start_pos;
    int visited[MAX_REQUESTS] = {0};

    for (int serviced = 0; serviced < request_count; serviced++)
    {
        int best_index = -1;
        int best_distance = 0;

        for (int i = 0; i < request_count; i++)
        {
            if (visited[i])
            {
                continue;
            }

            int distance = abs(current - requests[i]);
            if (best_index == -1 || distance < best_distance)
            {
                best_index = i;
                best_distance = distance;
            }
        }

        visited[best_index] = 1;
        total_seek += best_distance;
        current = requests[best_index];
    }

    return total_seek;
}

// Small local sort helper for LOOK/C-LOOK traversal order.
static void insertion_sort(int values[], int count)
{
    for (int i = 1; i < count; i++)
    {
        int key = values[i];
        int j = i - 1;

        while (j >= 0 && values[j] > key)
        {
            values[j + 1] = values[j];
            j--;
        }

        values[j + 1] = key;
    }
}

// LOOK: move upward first, then reverse once to service lower requests.
int look_total_seek(const int requests[], int request_count, int start_pos)
{
    int sorted[MAX_REQUESTS];
    int total_seek = 0;
    int current = start_pos;
    int split_index = 0;

    for (int i = 0; i < request_count; i++)
    {
        sorted[i] = requests[i];
    }

    insertion_sort(sorted, request_count);

    while (split_index < request_count && sorted[split_index] < start_pos)
    {
        split_index++;
    }

    for (int i = split_index; i < request_count; i++)
    {
        total_seek += abs(current - sorted[i]);
        current = sorted[i];
    }

    for (int i = split_index - 1; i >= 0; i--)
    {
        total_seek += abs(current - sorted[i]);
        current = sorted[i];
    }

    return total_seek;
}

// C-LOOK: move upward, wrap to lowest request, then continue upward.
int clook_total_seek(const int requests[], int request_count, int start_pos)
{
    int sorted[MAX_REQUESTS];
    int total_seek = 0;
    int current = start_pos;
    int split_index = 0;

    for (int i = 0; i < request_count; i++)
    {
        sorted[i] = requests[i];
    }

    insertion_sort(sorted, request_count);

    while (split_index < request_count && sorted[split_index] < start_pos)
    {
        split_index++;
    }

    for (int i = split_index; i < request_count; i++)
    {
        total_seek += abs(current - sorted[i]);
        current = sorted[i];
    }

    if (split_index > 0)
    {
        total_seek += abs(current - sorted[0]);
        current = sorted[0];

        for (int i = 1; i < split_index; i++)
        {
            total_seek += abs(current - sorted[i]);
            current = sorted[i];
        }
    }

    return total_seek;
}

int main(void)
{
    int values[MAX_REQUESTS + 1];
    int value_count = 0;

    // Read: first value is start position, remaining are block requests.
    while (value_count < MAX_REQUESTS + 1 && scanf("%d", &values[value_count]) == 1)
    {
        value_count++;
    }

    if (value_count < 2)
    {
        fprintf(stderr, "Error: provide at least a start position and one request.\n");
        return 1;
    }

    int start_pos = values[0];
    int request_count = value_count - 1;
    int requests[MAX_REQUESTS];

    for (int i = 0; i < request_count; i++)
    {
        requests[i] = values[i + 1];
    }

    int fcfs = fcfs_total_seek(requests, request_count, start_pos);
    int sstf = sstf_total_seek(requests, request_count, start_pos);
    int look = look_total_seek(requests, request_count, start_pos);
    int clook = clook_total_seek(requests, request_count, start_pos);

    printf("Assignment 7: Block Access Algorithm\n");
    printf("By: Ethan Hess\n\n");
    printf("FCFS Total Seek: %d\n", fcfs);
    printf("SSTF Total Seek: %d\n", sstf);
    printf("LOOK Total Seek: %d\n", look);
    printf("C-LOOK Total Seek: %d\n", clook);

    return 0;
}