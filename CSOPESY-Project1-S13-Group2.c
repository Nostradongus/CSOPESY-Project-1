/**
 * @file CSOPESY-Project1-S13-Group2.c
 * @author Angel Lopez (angel_lopez_a@dlsu.edu.ph)
 * @author Joshue Jadie (joshue_jadie@dlsu.edu.ph)
 * @date 29-04-22
 * @version 1.0
 *
 * CSOPESY S13
 * Project 1 - CPU Scheduling
 */

#include <stdio.h>
#include <stdlib.h>

// data structure for run time (start time to end time)
typedef struct RunTime {
    int startTime;
    int endTime;
} RunTime;

// data structure for a single process
typedef struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int waitingTime;
    RunTime* runTimes;
    int numOfRunTimes;
} Process;

// First-Come First-Serve Scheduling Algorithm
Process* FCFS (Process* arr, int numOfProcess) {
    // initial time
    int time = 0;

    // results array; sorted by waiting time
    Process* results = malloc(sizeof(Process) * numOfProcess);
    // results array index
    int index = 0;

    // execute FCFS algorithm
    while (index != numOfProcess) {
        // first process is the current chosen process for now
        Process p = arr[0];
        int curr = 0; // index for current chosen process
        // get process that has the current shortest arrival time and is not yet executed
        for (int i = 1; i < numOfProcess; i++) {
            // if current process process is already executed
            if (p.burstTime == 0) {
                p = arr[i];
                curr = i;
            }
            // else if current process has already arrived in waiting queue, and has 
            // shorter arrival time compared to current chosen process and 
            // it is not yet executed (burst time != 0)
            else if (arr[i].arrivalTime <= time && arr[i].arrivalTime < p.arrivalTime && arr[i].burstTime != 0) {
                p = arr[i];
                curr = i;
            }
        }

        // execute current chosen process
        p.waitingTime = time - p.arrivalTime;
        p.runTimes = realloc(NULL, sizeof(RunTime)); // or malloc(sizeof(RunTime))
        (*p.runTimes).startTime = time;
        (*p.runTimes).endTime = time + p.burstTime;
        p.numOfRunTimes++;
        time += p.burstTime; // update time
        p.burstTime = 0;     // done executing

        // update process array with executed process
        arr[curr] = p;

        // store executed process to results
        results[index] = p;
        index++;
    }

    return results;
}

// Shortest-Job First Scheduling Algorithm
Process* SJF (Process* arr, int numOfProcess) {
    // initial time
    int time = 0;

    // results array; sorted by waiting time
    Process* results = malloc(sizeof(Process) * numOfProcess);
    // results array index
    int index = 0;

    // execute SJF algorithm
    while (index != numOfProcess) {
        // get process with shortest burst time
        Process p = arr[0]; // first process is the current chosen process for now
        int curr = 0; // index for current chosen process
        for (int i = 1; i < numOfProcess; i++) {
            // if chosen process is already executed
            if (p.burstTime == 0) {
                p = arr[i];
                curr = i;
            }
            // else if current process has already arrived in waiting queue and
            // has shorter burst time compared to current chosen process and
            // it is not yet executed (burst time != 0)
            else if (arr[i].arrivalTime <= time && arr[i].burstTime < p.burstTime && arr[i].burstTime != 0) {
                p = arr[i];
                curr = i;
            }
            // else if both process have the same burst time but current process has arrived earlier
            // then current process must execute first
            else if (arr[i].burstTime == p.burstTime && arr[i].arrivalTime < p.arrivalTime) {
                p = arr[i];
                curr = i;
            }
        }

        // execute current chosen process
        p.waitingTime = time - p.arrivalTime;
        p.runTimes = realloc(NULL, sizeof(RunTime)); // or malloc(sizeof(RunTime))
        (*p.runTimes).startTime = time;
        (*p.runTimes).endTime = time + p.burstTime;
        p.numOfRunTimes++;
        time += p.burstTime; // update time
        p.burstTime = 0; // done executing

        // update process array with executed process
        arr[curr] = p;

        // store executed process to results 
        results[index] = p; 
        index++;
    }

    return results;
}

// Shortest-Remaining-Time-First Scheduling Algorithm
Process* SRTF (Process* arr, int numOfProcess) {
    // TODO: complete code
    return NULL;
}

// Round-Robin Scheduling Algorithm 
Process* RR (Process* arr, int numOfProcess) {
    // TODO: complete code
    return NULL;
}

int main (int argc, char* argv[]) {
    // file variables
    char filename[255];
    FILE *file;

    // get filename
    printf("Input filename: ");
    scanf("%s", filename);

    // check if file exists
    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "%s not found.", filename);
        exit(1);
    }

    // get CPU scheduling algorithm (X), number of processes (Y), and quantum value (Z) (Round-Robin only)
    int X, Y, Z;
    char cDump; // to consume newline per input row
    if ((fscanf(file, "%d %d %d%c", &X, &Y, &Z, &cDump)) == -1) {
        fprintf(stderr, "Error in %s file!", filename);
        exit(1);
    }

    // get all processes; process ID, arrival time, and burst time
    Process* processes = malloc(sizeof(Process) * Y);
    int id, arrivalTime, burstTime; // for current inputs
    for (int i = 0; i < Y; i++) {
        // get current process input
        if ((fscanf(file, "%d %d %d%c", &id, &arrivalTime, &burstTime, &cDump)) == -1) {
            fprintf(stderr, "Error in %s file! Might be in bad format!", filename);
            exit(1);
        }
        // store the current process input in array
        processes[i].id = id;
        processes[i].arrivalTime = arrivalTime; 
        processes[i].burstTime = burstTime;
        processes[i].waitingTime = 0; // always zero at first
        processes[i].runTimes = NULL;
        processes[i].numOfRunTimes = 0; // always zero at first
    }

    // close file after getting all needed inputs
    fclose(file);

    // execute scheduling algorithms based on value of X
    // get the results sorted by waiting time in ascending order
    Process* results = NULL;
    switch (X) {
        case 0:
            // perform FCFS
            results = FCFS(processes, Y);
            break;
        case 1:
            // perform SJF
            results = SJF(processes, Y);
            break;
        case 2: 
            // perform SRTF
            results = SRTF(processes, Y);
            break;
        case 3:
            // perform RR 
            results = RR(processes, Y);
            break;
        default: 
            // invalid X value, no scheduling algorithm can be used
            printf("Invalid X value!");
            exit(1);
    }

    // display results
    for (int i = 0; i < Y; i++) {
        Process p = results[i];
        printf("P[%d] ", p.id);
        for (int j = 0; j < p.numOfRunTimes; j++) {
            printf("Start Time: %d End Time: %d | ", p.runTimes[j].startTime, p.runTimes[j].endTime);
        }
        printf("Waiting time: %d\n", p.waitingTime);
    }

    // compute and display average waiting time of the processes
    double sum = 0.0;
    for (int i = 0; i < Y; i++) {
        sum += (double) processes[i].waitingTime;
    }
    printf("Average waiting time: %.2f", sum / (double) Y);

    return 0;
}