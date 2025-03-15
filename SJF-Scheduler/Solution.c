// Include the required header files
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

// Define the required data types
typedef struct process {
    int id;
    int arrivalTime;
    int burstTime;
    int turnAroundTime;
    int waitTime;
    int responseTime;
} process;
typedef struct execution {
    int startTime;
    int endTime;
    int processId;
} execution;
typedef struct schedule {
    int scheduleSize;
    execution *executions;
    float averageWaitTime;
    float averageResponseTime;
    float averageTurnAroundTime;
} schedule;
typedef struct heap {
    int heapSize;
    process *heapArray;
} heap;

/*
Name        : swap()
Description : Swaps two processes
Arguments   : The two processes to be swapped
Return      : None
*/
void swap(process *p1, process *p2) {
    process p = (*p1);
    (*p1) = (*p2);
    (*p2) = p;
    return;
}

/*
Name        : push()
Description : Pushes a process into a heap
Arguments   : A pointer to the heap and the process to be pushed
Return      : None
*/
void push(heap *next, process p) {

    // Declare the required local variables
    int index, parent;
    bool heap;
    process v;

    // Push the process to the end of the heap array
    next->heapArray = (process*)realloc(next->heapArray, (next->heapSize + 1)*sizeof(process));
    next->heapArray[next->heapSize++] = p;

    // Heapify the array
    index = next->heapSize - 1;
    v = next->heapArray[index];
    heap = false;
    while(!heap && index > 0) {
        parent = (index & 1) ? (index / 2) : ((index - 1) / 2);
        if(next->heapArray[parent].arrivalTime < v.arrivalTime || 
            (next->heapArray[parent].arrivalTime == v.arrivalTime && 
                next->heapArray[parent].burstTime < v.burstTime) ||
            (next->heapArray[parent].arrivalTime == v.arrivalTime && 
                next->heapArray[parent].burstTime == v.burstTime &&  
                next->heapArray[parent].id < v.id)) {
            heap = true;
        }
        else {
            next->heapArray[index] = next->heapArray[parent];
            index = parent;
        }
    }
    next->heapArray[index] = v;

    // Return
    return;
}

/*
Name        : pop()
Description : Pops the top process from the heap array
Arguments   : A pointer to the heap
Return      : The popped process
*/
process pop(heap *next) {

    // Declare the required local variables
    int index, child;
    bool heap;
    process p, v;

    // Swap the first and last processes and remove it
    p = next->heapArray[0];
    swap(&next->heapArray[0], &next->heapArray[next->heapSize - 1]);
    next->heapSize--;

    // Heapify the array
    index = 0;
    v = next->heapArray[index];
    heap = false;
    while(!heap && 2*index + 1 < next->heapSize) {
        child = 2*index + 1;
        if(child + 1 < next->heapSize && (next->heapArray[child + 1].arrivalTime < next->heapArray[child].arrivalTime || 
            (next->heapArray[child + 1].arrivalTime == next->heapArray[child].arrivalTime && 
                next->heapArray[child + 1].burstTime < next->heapArray[child].burstTime) ||
            (next->heapArray[child + 1].arrivalTime == next->heapArray[child].arrivalTime && 
                next->heapArray[child + 1].burstTime == next->heapArray[child].burstTime &&
                next->heapArray[child + 1].id < next->heapArray[child].id)))
            child++;
        if(v.arrivalTime < next->heapArray[child].arrivalTime || 
            (v.arrivalTime == next->heapArray[child].arrivalTime && 
                v.burstTime < next->heapArray[child].burstTime) || 
            (v.arrivalTime == next->heapArray[child].arrivalTime && 
                v.burstTime == next->heapArray[child].burstTime &&
                v.id < next->heapArray[child].id))
            heap = true;
        else {
            next->heapArray[index] = next->heapArray[child];
            index = child;
        }
    }
    next->heapArray[index] = v;

    // Return
    return p;
}

/*
Name        : sjfScheduler()
Description : Computes the Shortest Job First schedule for the processes
Arguments   : An integer denoting the number of processes and an array of processes
Return      : The schedule
*/
schedule sjfScheduler(int numProcesses, process *processes) {

    // Declare the required local variables
    int time = 0;
    heap next = {0, NULL};
    schedule scheduling = {0, NULL, 0.0, 0.0, 0.0};
    next.heapArray = (process*)malloc(sizeof(process));
    scheduling.executions = (execution*)malloc(sizeof(execution));

    // Push all the processes into the heap
    for(int i = 0; i < numProcesses; i++)
        push(&next, processes[i]);
    
    // Pop the processes according to the rule
    while(next.heapSize > 0) {
        process p = pop(&next);
        scheduling.executions = (execution*)realloc(scheduling.executions, (scheduling.scheduleSize + 1)*sizeof(execution));
        scheduling.executions[scheduling.scheduleSize].processId = p.id;
        scheduling.executions[scheduling.scheduleSize].startTime = (time > p.arrivalTime) ? time : p.arrivalTime;
        scheduling.executions[scheduling.scheduleSize].endTime = scheduling.executions[scheduling.scheduleSize].startTime + p.burstTime;
        scheduling.averageResponseTime += scheduling.executions[scheduling.scheduleSize].startTime - p.arrivalTime;
        scheduling.averageWaitTime += scheduling.executions[scheduling.scheduleSize].startTime - p.arrivalTime;
        scheduling.averageTurnAroundTime = scheduling.averageWaitTime - p.burstTime;
        time = scheduling.executions[scheduling.scheduleSize].endTime;
        scheduling.scheduleSize++;
    }

    // Compute the averages
    scheduling.averageResponseTime /= numProcesses;
    scheduling.averageTurnAroundTime /= numProcesses;
    scheduling.averageWaitTime /= numProcesses;
    
    // Free the heap and return
    free(next.heapArray);
    return scheduling;
}

/*
Name        : main()
Description : Handles I/O and calls the required subroutines
Arguments   : None
Return      : Integer 0
*/
int main() {

    // Open the input file and handle the exception
    FILE *input = fopen("Input.txt", "r");
    if(!input) {
        perror("Unable to open the Input file...!!!");
        return 0;
    }

    // Declare the required variables and read the input
    int numProcesses;
    process *processes = NULL;
    fscanf(input, "%d", &numProcesses);
    processes = (process*)calloc(numProcesses, sizeof(process));
    for(int i = 0; i < numProcesses; i++) {
        fscanf(input, "%d %d %d", &processes[i].id, &processes[i].arrivalTime, &processes[i].burstTime);
        processes[i].waitTime = 0;
        processes[i].responseTime = 0;
        processes[i].turnAroundTime = 0;
    }

    // Close the input file
    fclose(input);
    input = NULL;

    // Call the subroutine to compute the First Come First Serve schedule
    schedule scheduling = sjfScheduler(numProcesses, processes);

    // Open the output file
    FILE *output = fopen("Output.txt", "w");

    // Write the output to the output file
    fprintf(output, "Start Time\t\tEnd Time\tProcess ID");
    for(int i = 0; i < scheduling.scheduleSize; i++)
        fprintf(output, "\n%d\t\t\t\t%d\t\t\t%d", scheduling.executions[i].startTime, scheduling.executions[i].endTime, scheduling.executions[i].processId);
    fprintf(output, "\nAverage Waiting Time: %.4f", scheduling.averageWaitTime);
    fprintf(output, "\nAverage Response Time: %.4f", scheduling.averageResponseTime);
    fprintf(output, "\nAverage Turn Around Time: %.4f", scheduling.averageTurnAroundTime);
    
    // Free the memory, close the output file and return
    free(processes);
    free(scheduling.executions);
    processes = NULL;
    scheduling.executions = NULL;
    fclose(output);
    return 0;
}