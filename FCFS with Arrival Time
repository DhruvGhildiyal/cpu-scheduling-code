#include <iostream>
#include <vector>
#include <iomanip>  // For std::setw
#include <algorithm> // For std::sort

// Structure to represent a process (now includes arrival time)
struct ProcessWithArrival {
    int pid;            // Process ID
    int arrivalTime;    // Arrival Time
    int burstTime;      // Burst Time
    int waitingTime;    // Waiting Time
    int turnaroundTime; // Turnaround Time
    int completionTime; // Completion Time
    int startTime;      // Time execution starts
};

// Comparison function for sorting processes based on arrival time
bool compareArrival(const ProcessWithArrival& a, const ProcessWithArrival& b) {
    if (a.arrivalTime != b.arrivalTime) {
        return a.arrivalTime < b.arrivalTime; // Sort primarily by arrival time
    }
    // Optional: Tie-breaking using PID if arrival times are the same
    // return a.pid < b.pid;
    return false; // Keep original relative order if arrival times are equal (stable sort behavior)
}

// Function to calculate FCFS metrics with arrival time
void calculateFcfsWithArrival(std::vector<ProcessWithArrival>& processes) {
    int n = processes.size();
    if (n == 0) return;

    // Sort processes based on arrival time
    std::sort(processes.begin(), processes.end(), compareArrival);

    int currentTime = 0; // Initialize current time

    for (int i = 0; i < n; ++i) {
        // Check for CPU idle time
        if (currentTime < processes[i].arrivalTime) {
            currentTime = processes[i].arrivalTime; // Fast-forward time to process arrival
        }

        // Calculate metrics for the current process
        processes[i].startTime = currentTime;
        processes[i].completionTime = currentTime + processes[i].burstTime;
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;

        // Waiting time cannot be negative (handles edge case if TAT < BT, though unlikely with FCFS)
        if (processes[i].waitingTime < 0) {
            processes[i].waitingTime = 0;
        }

        // Update current time for the next process
        currentTime = processes[i].completionTime;
    }
}

// Function to print the results table (adapted for arrival time)
void printResultsWithArrival(const std::vector<ProcessWithArrival>& processes) {
    int n = processes.size();
     if (n == 0) {
        std::cout << "No processes to display." << std::endl;
        return;
    }

    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;

    // Store original PIDs if needed after sorting
    std::vector<ProcessWithArrival> sorted_by_pid = processes;
    std::sort(sorted_by_pid.begin(), sorted_by_pid.end(), [](const ProcessWithArrival& a, const ProcessWithArrival& b){
        return a.pid < b.pid;
    });


    // Print table header
    std::cout << std::setw(5) << "PID"
              << std::setw(15) << "Arrival Time"
              << std::setw(15) << "Burst Time"
              << std::setw(15) << "Start Time" // Added Start Time for clarity
              << std::setw(15) << "Waiting Time"
              << std::setw(18) << "Turnaround Time"
              << std::setw(18) << "Completion Time" << std::endl;
    std::cout << "---------------------------------------------------------------------------------------------" << std::endl;


    // Print process details (sorted by PID for consistent output)
    for (const auto& p : sorted_by_pid) {
        std::cout << std::setw(5) << p.pid
                  << std::setw(15) << p.arrivalTime
                  << std::setw(15) << p.burstTime
                  << std::setw(15) << p.startTime
                  << std::setw(15) << p.waitingTime
                  << std::setw(18) << p.turnaroundTime
                  << std::setw(18) << p.completionTime << std::endl;

        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
    }
     std::cout << "---------------------------------------------------------------------------------------------" << std::endl;

    // Print averages
    std::cout << std::fixed << std::setprecision(2); // Set precision for averages
    std::cout << "\nAverage Waiting Time: " << totalWaitingTime / n << std::endl;
    std::cout << "Average Turnaround Time: " << totalTurnaroundTime / n << std::endl;
}

void runFcfsWithArrival() {
    int n;
    std::cout << "\n--- FCFS Scheduling (With Arrival Time) ---" << std::endl;
    std::cout << "Enter the number of processes: ";
    std::cin >> n;

    if (n <= 0) {
         std::cout << "Number of processes must be positive." << std::endl;
         return;
    }


    std::vector<ProcessWithArrival> processes(n);
    std::cout << "Enter Arrival Time and Burst Time for each process:" << std::endl;
    for (int i = 0; i < n; ++i) {
        processes[i].pid = i + 1;
        std::cout << "Process " << processes[i].pid << " Arrival Time: ";
        std::cin >> processes[i].arrivalTime;
        std::cout << "Process " << processes[i].pid << " Burst Time: ";
        std::cin >> processes[i].burstTime;

         if (processes[i].arrivalTime < 0) {
             std::cout << "Arrival time cannot be negative. Setting to 0." << std::endl;
             processes[i].arrivalTime = 0;
         }
        if (processes[i].burstTime <= 0) {
             std::cout << "Burst time must be positive. Setting to 1." << std::endl;
             processes[i].burstTime = 1; // Basic error handling
        }
    }

    calculateFcfsWithArrival(processes);
    std::cout << "\n--- FCFS Results (With Arrival Time) ---" << std::endl;
    printResultsWithArrival(processes);
}

// int main() {
//     runFcfsWithArrival();
//     return 0;
// }
