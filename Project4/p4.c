#include <iostream>
#include <iomanip>
#include <queue>
#include <vector>
#include <string>
#include <cstring>
#include <climits>
#include <cstdio>
using namespace std;

const int MAX_PROCESSES = 100;

struct Process
{
    int arrival;
    int burst;
    int id;
};

// Helper function to print statistics
void printStats(const string &algorithmName, double totalResponse, double totalWait, double totalTurnaround, int numProcesses)
{
    cout << algorithmName << "\n";
    cout << fixed << setprecision(2);
    cout << "Avg. Resp.:" << (totalResponse / numProcesses)
         << ", Avg. T.A.:" << (totalTurnaround / numProcesses)
         << ", Avg. Wait:" << (totalWait / numProcesses) << "\n\n";
}

// Helper function to find next unprocessed process by earliest arrival
int findNextProcess(const vector<bool> &processed, const Process processes[], int numProcesses, int &clock)
{
    int nextProcess = -1;

    for (int j = 0; j < numProcesses; j++)
    {
        if (!processed[j])
        {
            if (nextProcess == -1 || processes[j].arrival < processes[nextProcess].arrival)
            {
                nextProcess = j;
            }
        }
    }

    clock = processes[nextProcess].arrival;
    return nextProcess;
}

// Helper function to calculate and print statistics
void calculateAndPrintStats(const string &algorithmName, const vector<int> &responseTime,
                            const vector<int> &finishTime, const Process processes[],
                            int numProcesses)
{
    double totalResponse = 0, totalWait = 0, totalTurnaround = 0;
    for (int i = 0; i < numProcesses; i++)
    {
        totalResponse += responseTime[i];
        int turnaroundTime = finishTime[i] - processes[i].arrival;
        int waitTime = turnaroundTime - processes[i].burst;
        totalWait += waitTime;
        totalTurnaround += turnaroundTime;
    }

    printStats(algorithmName, totalResponse, totalWait, totalTurnaround, numProcesses);
}

// FCFS Scheduling
void firstComeFirstServed(Process processes[], int numProcesses)
{
    int clock = 0;
    double totalResponse = 0, totalWait = 0, totalTurnaround = 0;

    for (int i = 0; i < numProcesses; i++)
    {
        // Process starts at max(current clock, arrival time)
        int startTime = max(clock, processes[i].arrival);
        int responseTime = startTime - processes[i].arrival;
        int finishTime = startTime + processes[i].burst;
        int turnaroundTime = finishTime - processes[i].arrival;
        int waitTime = turnaroundTime - processes[i].burst;

        totalResponse += responseTime;
        totalWait += waitTime;
        totalTurnaround += turnaroundTime;

        clock = finishTime;
    }

    printStats("First Come, First Served", totalResponse, totalWait, totalTurnaround, numProcesses);
}

// SJF Scheduling
void shortestJobFirst(Process processes[], int numProcesses)
{
    vector<bool> executed(numProcesses, false);
    int clock = 0;
    double totalResponse = 0, totalWait = 0, totalTurnaround = 0;

    for (int i = 0; i < numProcesses; i++)
    {
        int nextProcess = -1;
        int minBurst = INT_MAX;

        // Find the process with minimum burst time that has arrived and hasn't executed
        for (int j = 0; j < numProcesses; j++)
        {
            if (!executed[j] && processes[j].arrival <= clock && processes[j].burst < minBurst)
            {
                nextProcess = j;
                minBurst = processes[j].burst;
            }
        }

        // If no process has arrived yet, jump to the next arrival
        if (nextProcess == -1)
        {
            nextProcess = findNextProcess(executed, processes, numProcesses, clock);
        }

        int startTime = max(clock, processes[nextProcess].arrival);
        int responseTime = startTime - processes[nextProcess].arrival;
        int finishTime = startTime + processes[nextProcess].burst;
        int turnaroundTime = finishTime - processes[nextProcess].arrival;
        int waitTime = turnaroundTime - processes[nextProcess].burst;

        totalResponse += responseTime;
        totalWait += waitTime;
        totalTurnaround += turnaroundTime;

        executed[nextProcess] = true;
        clock = finishTime;
    }

    printStats("Shortest Job First", totalResponse, totalWait, totalTurnaround, numProcesses);
}

// SRTF Scheduling
void shortestRemainingTimeFirst(Process processes[], int numProcesses)
{
    vector<int> remaining(numProcesses);
    vector<bool> finished(numProcesses, false);
    vector<int> responseTime(numProcesses, -1);
    vector<int> finishTime(numProcesses);

    for (int i = 0; i < numProcesses; i++)
    {
        remaining[i] = processes[i].burst;
    }

    int clock = 0;
    int completed = 0;

    while (completed < numProcesses)
    {
        // Find the process with minimum remaining time that has arrived and not finished
        int nextProcess = -1;
        int minRemaining = INT_MAX;

        for (int j = 0; j < numProcesses; j++)
        {
            if (!finished[j] && processes[j].arrival <= clock && remaining[j] < minRemaining)
            {
                nextProcess = j;
                minRemaining = remaining[j];
            }
        }

        // If no process has arrived yet, jump to next arrival
        if (nextProcess == -1)
        {
            nextProcess = findNextProcess(finished, processes, numProcesses, clock);
            continue;
        }

        // Record response time on first execution
        if (responseTime[nextProcess] == -1)
        {
            responseTime[nextProcess] = clock - processes[nextProcess].arrival;
        }

        // Find next event: either this process finishes or another process arrives
        int nextArrival = INT_MAX;
        for (int j = 0; j < numProcesses; j++)
        {
            if (!finished[j] && processes[j].arrival > clock)
            {
                nextArrival = min(nextArrival, processes[j].arrival);
            }
        }

        int timeToRun = min(remaining[nextProcess], nextArrival - clock);
        clock += timeToRun;
        remaining[nextProcess] -= timeToRun;

        if (remaining[nextProcess] == 0)
        {
            finished[nextProcess] = true;
            finishTime[nextProcess] = clock;
            completed++;
        }
    }

    calculateAndPrintStats("Shortest Remaining Time First", responseTime, finishTime, processes, numProcesses);
}

// Round Robin Scheduling
void roundRobin(Process processes[], int numProcesses, int timeQuantum)
{
    queue<int> readyQueue;
    vector<int> remaining(numProcesses);
    vector<int> responseTime(numProcesses, -1);
    vector<int> finishTime(numProcesses);

    for (int i = 0; i < numProcesses; i++)
    {
        remaining[i] = processes[i].burst;
    }

    int clock = 0;
    int completed = 0;
    int nextToArrive = 0;

    while (completed < numProcesses)
    {
        // Add all processes that have arrived by this time
        while (nextToArrive < numProcesses && processes[nextToArrive].arrival <= clock)
        {
            readyQueue.push(nextToArrive);
            nextToArrive++;
        }

        if (readyQueue.empty())
        {
            // Jump to next arrival time
            clock = processes[nextToArrive].arrival;
            continue;
        }

        int currentProcess = readyQueue.front();
        readyQueue.pop();

        // Record response time on first execution
        if (responseTime[currentProcess] == -1)
        {
            responseTime[currentProcess] = clock - processes[currentProcess].arrival;
        }

        // Determine how much time this process will use this turn
        int timeToRun = min(remaining[currentProcess], timeQuantum);
        int endTime = clock + timeToRun;

        // Check which processes arrive during this time slice
        vector<int> newArrivals;
        while (nextToArrive < numProcesses && processes[nextToArrive].arrival <= endTime)
        {
            newArrivals.push_back(nextToArrive);
            nextToArrive++;
        }

        clock = endTime;
        remaining[currentProcess] -= timeToRun;

        // Add newly arrived processes to queue
        for (int pid : newArrivals)
        {
            readyQueue.push(pid);
        }

        if (remaining[currentProcess] == 0)
        {
            // Process finished
            finishTime[currentProcess] = clock;
            completed++;
        }
        else
        {
            // Add current process back to end of queue
            readyQueue.push(currentProcess);
        }
    }

    char rrName[50];
    sprintf(rrName, "Round Robin with Time Quantum of %d", timeQuantum);
    calculateAndPrintStats(string(rrName), responseTime, finishTime, processes, numProcesses);
}

int main(int argc, char *argv[])
{
    int timeQuantum = 100; // Default time quantum

    // Parse command line argument for time quantum
    if (argc > 1)
    {
        timeQuantum = atoi(argv[1]);
    }

    Process processes[MAX_PROCESSES];
    int numProcesses = 0;

    // Read input from stdin
    int arrival, burst;
    while (cin >> arrival >> burst && numProcesses < MAX_PROCESSES)
    {
        processes[numProcesses].arrival = arrival;
        processes[numProcesses].burst = burst;
        processes[numProcesses].id = numProcesses;
        numProcesses++;
    }

    // Run all four scheduling algorithms
    firstComeFirstServed(processes, numProcesses);
    shortestJobFirst(processes, numProcesses);
    shortestRemainingTimeFirst(processes, numProcesses);
    roundRobin(processes, numProcesses, timeQuantum);

    return 0;
}
