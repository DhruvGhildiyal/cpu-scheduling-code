#include <iostream>
#include <vector>
#include <iomanip> // For std::setw

// Structure to represent a process
struct Process {
    int pid;       // Process ID
    int burstTime; // Burst Time
    int waitingTime; // Waiting Time
    int turnaroundTime; // Turnaround Time
    int completionTime; // Completion Time
};

// Function to calculate FCFS metrics without arrival time
void calculateFcfsWithoutArrival(std::vector<Process>& processes) {
    int n = processes.size();
    if (n == 0) return;

    int currentTime = 0; // Initialize current time

    for (int i = 0; i < n; ++i) {
        // Waiting time for the current process is the completion time of the previous one
        processes[i].waitingTime = currentTime;

        // Completion time is current time + burst time
        processes[i].completionTime = currentTime + processes[i].burstTime;

        // Turnaround time is completion time (since arrival time is 0)
        processes[i].turnaroundTime = processes[i].completionTime;

        // Update current time for the next process
        currentTime = processes[i].completionTime;
    }
}

// Function to print the results table
void printResults(const std::vector<Process>& processes, bool showArrivalTime = false) {
    int n = processes.size();
    if (n == 0) {
        std::cout << "No processes to display." << std::endl;
        return;
    }

    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;

    // Print table header
    std::cout << std::setw(5) << "PID" << std::setw(15) << "Burst Time";
    if (showArrivalTime) {
        std::cout << std::setw(15) << "Arrival Time";
    }
    std::cout << std::setw(15) << "Waiting Time"
              << std::setw(18) << "Turnaround Time"
              << std::setw(18) << "Completion Time" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;


    // Print process details
    for (const auto& p : processes) {
        std::cout << std::setw(5) << p.pid
                  << std::setw(15) << p.burstTime;
        if (showArrivalTime) {
             std::cout << std::setw(15) << 0; // Arrival time is 0
        }
        std::cout << std::setw(15) << p.waitingTime
                  << std::setw(18) << p.turnaroundTime
                  << std::setw(18) << p.completionTime << std::endl;

        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
    }
     std::cout << "--------------------------------------------------------------------------------" << std::endl;


    // Print averages
    std::cout << std::fixed << std::setprecision(2); // Set precision for averages
    std::cout << "\nAverage Waiting Time: " << totalWaitingTime / n << std::endl;
    std::cout << "Average Turnaround Time: " << totalTurnaroundTime / n << std::endl;
}

void runFcfsWithoutArrival() {
    int n;
    std::cout << "\n--- FCFS Scheduling (Without Arrival Time) ---" << std::endl;
    std::cout << "Enter the number of processes: ";
    std::cin >> n;

    if (n <= 0) {
         std::cout << "Number of processes must be positive." << std::endl;
         return;
    }

    std::vector<Process> processes(n);
    std::cout << "Enter Burst Time for each process:" << std::endl;
    for (int i = 0; i < n; ++i) {
        processes[i].pid = i + 1;
        std::cout << "Process " << processes[i].pid << " Burst Time: ";
        std::cin >> processes[i].burstTime;
        if (processes[i].burstTime <= 0) {
             std::cout << "Burst time must be positive. Setting to 1." << std::endl;
             processes[i].burstTime = 1; // Basic error handling
        }
    }

    calculateFcfsWithoutArrival(processes);
    std::cout << "\n--- FCFS Results (Without Arrival Time) ---" << std::endl;
    printResults(processes, false); // Don't show arrival time column
}

// int main() {
//     runFcfsWithoutArrival();
//     return 0;
// }
