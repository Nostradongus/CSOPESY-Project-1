/**
 * @file CSOPESY-Project1-S13-Group2.c
 * @author Angel Lopez (angel_lopez_a@dlsu.edu.ph)
 * @author Joshue Jadie (joshue_jadie@dlsu.edu.ph)
 * @date 10-05-22
 *
 * CSOPESY S13
 * Project 1 - CPU Scheduling
 */

#include <stdio.h>
#include <stdlib.h>

// Data structure for run time (start time to end time)
typedef struct RunTime {
    int startTime;
    int endTime;
} RunTime;

// Data structure for a single process
typedef struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int waitingTime;
    RunTime* runTimes;
    int numOfRunTimes;
} Process;

// Scheduling algorithms
Process* FCFS (Process* arr, int numOfProcess);
Process* SJF (Process* arr, int numOfProcess);
Process* SRTF (Process* arr, int numOfProcess);
Process* RR(Process *arr, int numOfProcess, int quantum);

int main(int argc, char *argv[]) {
    // file variables
    char filename[255];
    FILE *file;

    // get filename
    printf("Input filename (w/ file extension): ");
    scanf("%s", filename);

    // check if file exists
    if ((file = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "%s not found.", filename);
        exit(1);
    }

    // get CPU scheduling algorithm (X), number of processes (Y), and quantum value (Z) (Round-Robin only)
    int X, Y, Z;
    char cDump; // to consume newline per input row
    if ((fscanf(file, "%d %d %d%c", &X, &Y, &Z, &cDump)) == -1)
    {
        fprintf(stderr, "Error in %s file!", filename);
        exit(1);
    }

    // get all processes; process ID, arrival time, and burst time
    Process *processes = malloc(sizeof(Process) * Y);
    int id, arrivalTime, burstTime; // for current inputs
    for (int i = 0; i < Y; i++)
    {
        // get current process input
        if ((fscanf(file, "%d %d %d%c", &id, &arrivalTime, &burstTime, &cDump)) == -1)
        {
            fprintf(stderr, "Error in %s file! Might have bad formatting!", filename);
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
    // get the results (sorted by order of finished processes)
    Process *results = NULL;
    switch (X)
    {
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
        results = RR(processes, Y, Z);
        break;
    default:
        // invalid X value, no scheduling algorithm can be used
        printf("Invalid X value! Please try again.");
        exit(1);
    }

    // display results
    for (int i = 0; i < Y; i++)
    {
        Process p = results[i];
        printf("P[%d] ", p.id);
        for (int j = 0; j < p.numOfRunTimes; j++)
        {
            printf("Start Time: %d End Time: %d | ", p.runTimes[j].startTime, p.runTimes[j].endTime);
        }
        printf("Waiting time: %d\n", p.waitingTime);
    }

    // compute and display average waiting time from all processes
    double sum = 0.0;
    for (int i = 0; i < Y; i++)
    {
        sum += (double)processes[i].waitingTime;
    }
    printf("Average waiting time: %.1f", sum / (double) Y);

    return 0;
}

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
            // if current process is already executed or has not yet arrived
            if (p.burstTime == 0 || p.arrivalTime > time) {
                p = arr[i];
                curr = i;
            }
            // else if current process has already arrived in waiting queue, and has 
            // shorter arrival time compared to current chosen process and 
            // it is not yet executed completely (burst time != 0)
            else if (arr[i].arrivalTime <= time && arr[i].arrivalTime < p.arrivalTime && arr[i].burstTime != 0) {
                p = arr[i];
                curr = i;
            }
        }

        // if current chosen process has already arrived and is not yet completely executed
        if (p.arrivalTime <= time && p.burstTime > 0) {
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
        } else { // else if no new process has arrived yet and all arrived processes are already completed
            // move time 
            time++;
        }
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
        Process p = arr[0]; // first process is the current chosen process for now
        int curr = 0; // index for current chosen process

        // get process with shortest burst time
        for (int i = 1; i < numOfProcess; i++) {
            // if chosen process is already executed or has not yet arrived
            if (p.burstTime == 0 || p.arrivalTime > time) {
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

        // if current chosen process has already arrived and is not yet completely executed
        if (p.arrivalTime <= time && p.burstTime > 0) {
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
        } else { // else, no new process has arrived yet and all arrived processes are already completed
            // move time 
            time++;
        }
    }

    return results;
}

// Shortest-Remaining-Time-First Scheduling Algorithm
Process* SRTF (Process* arr, int numOfProcess) {
    // initial time
    int time = 0;

    // results array; sorted by waiting time 
    Process* results = malloc(sizeof(Process) * numOfProcess);
    // results array index
    int index = 0;

    // execute SRTF algorithm
    while (index != numOfProcess) {
        Process p = arr[0]; // first process is the current chosen process for now
        int curr = 0; // index for current chosen process

        // get process with shortest burst time
        for (int i = 1; i < numOfProcess; i++) {
            // if chosen process is already fully executed or has not yet arrived
            if (p.burstTime == 0 || p.arrivalTime > time) {
                p = arr[i];
                curr = i;
            }
            // else if current process has already arrived in waiting queue and
            // has shorter burst time compared to current process and
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

        // if current chosen process has already arrived and is not yet completely executed
        if (p.arrivalTime <= time && p.burstTime > 0) {
            // execute current chosen process
            p.waitingTime += time - p.arrivalTime;
            p.numOfRunTimes++;
            p.runTimes = realloc(p.runTimes, sizeof(RunTime) * p.numOfRunTimes);
            p.runTimes[p.numOfRunTimes - 1].startTime = time;
            // stop executing process if new process with shorter burst time has arrived
            // or current chosen process has been fully executed (burst time = 0)
            int stop = 0;
            while (!stop && p.burstTime > 0) {
                // increase time and decrease burst time of current chosen process
                time++;
                p.burstTime--;
                // check if there are new arrived processes with shorter burst time
                for (int i = 0; i < numOfProcess; i++) {
                    if (arr[i].arrivalTime == time && arr[i].burstTime < p.burstTime && arr[i].burstTime != 0) {
                        // stop executing current chosen process
                        stop = 1;
                    }
                }
            }
            p.runTimes[p.numOfRunTimes - 1].endTime = time;
            p.arrivalTime = time; // update arrival time of current chosen process for next execution

            // update process array with current chosen process
            arr[curr] = p;

            // if current chosen process is already completed (burst time = 0), store to results
            if (p.burstTime == 0) {
                results[index] = p;
                index++;
            }
        } else { // else, no new process has arrived yet and all arrived processes are already completed
            // move time
            time++;
        }
    }

    return results;
}

// Round-Robin Scheduling Algorithm 
Process* RR (Process* arr, int numOfProcess, int quantum) {
    // initial time
    int time = 0;

    // results array; sorted by waiting time
    Process* results = malloc(sizeof(Process) * numOfProcess);
    // results array index
    int index = 0;

    // execute RR algorithm
    int curr = 0; // current chosen process index
    while (index != numOfProcess) {
        // get current chosen process to be executed
        Process p = arr[curr];
        
        // if current chosen process has already arrived and is not yet completely executed
        if (p.arrivalTime <= time && p.burstTime > 0) {
            // execute current chosen process
            p.waitingTime += time - p.arrivalTime;
            p.numOfRunTimes++;
            p.runTimes = realloc(p.runTimes, sizeof(RunTime) * p.numOfRunTimes);
            p.runTimes[p.numOfRunTimes - 1].startTime = time;
            time = (p.burstTime < quantum ? time + p.burstTime : time + quantum); // move time 
            p.burstTime -= quantum; // execute process by time slice value
            p.runTimes[p.numOfRunTimes - 1].endTime = time;
            p.arrivalTime = time; // update arrival time of process for next execution

            // update process array with current chosen process
            arr[curr] = p;

            // if current chosen process is already completed (burst time = 0), store to results
            if (p.burstTime <= 0) {
                results[index] = p;
                index++;
            }

            // move to next process in the array
            curr = curr < numOfProcess - 1 ? curr + 1 : 0;
        } else if (p.burstTime <= 0) { // if current process is already done executing
            // move to next process in the array
            curr = curr < numOfProcess - 1 ? curr + 1 : 0;
        } else  { // else if no new process has arrived yet
            // check if all arrived processes are already completed
            int move = 1;
            for (int i = 0; i < numOfProcess; i++) {
                // if process has arrived and is not yet completed
                if (arr[i].arrivalTime <= time && arr[i].burstTime > 0) {
                    move = 0;
                }
            }

            // if all arrived processes are already completed, move time
            if (move) {
                time++;
            } else {
                // move to next process in the array
                curr = curr < numOfProcess - 1 ? curr + 1 : 0;
            }
        }
    }

    return results;
}
