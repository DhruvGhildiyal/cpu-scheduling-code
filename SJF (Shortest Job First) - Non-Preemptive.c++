#include <iostream>
#include <vector>
#include <algorithm> // For std::sort, std::min_element
#include <limits>    // For std::numeric_limits
#include <iomanip>   // For formatting output

// Structure to represent a process
struct Process {
    int id;             // Process ID
    int arrivalTime;    // Arrival Time (AT)
    int burstTime;      // Burst Time (BT)
    int completionTime; // Completion Time (CT)
    int turnaroundTime; // Turnaround Time (TAT)
    int waitingTime;    // Waiting Time (WT)
    bool isCompleted;   // Flag to check if process is finished

    // Constructor
    Process(int i = 0, int at = 0, int bt = 0)
        : id(i), arrivalTime(at), burstTime(bt),
          completionTime(0), turnaroundTime(0), waitingTime(0), isCompleted(false) {}
};

// Function to display results
void displayResults(const std::vector<Process>& processes) {
    int n = processes.size();
    if (n == 0) {
        std::cout << "No processes to display." << std::endl;
        return;
    }

    double totalWT = 0;
    double totalTAT = 0;

    // Optional: Sort by ID for consistent display order if needed,
    // but showing execution order might be more insightful sometimes.
    // std::vector<Process> sorted_processes = processes;
    // std::sort(sorted_processes.begin(), sorted_processes.end(),
    //           [](const Process& a, const Process& b){ return a.id < b.id; });

    std::cout << "\n--- SJF (Non-Preemptive) Schedule Results ---\n";
    std::cout << "---------------------------------------------------------------------------------\n";
    std::cout << std::setw(5) << "PID" << std::setw(15) << "Arrival Time"
              << std::setw(15) << "Burst Time"
              << std::setw(20) << "Completion Time"
              << std::setw(18) << "Turnaround Time"
              << std::setw(15) << "Waiting Time" << std::endl;
    std::cout << "---------------------------------------------------------------------------------\n";

    for (const auto& p : processes) { // Use original vector to show calc order
        std::cout << std::setw(5) << p.id
                  << std::setw(15) << p.arrivalTime
                  << std::setw(15) << p.burstTime
                  << std::setw(20) << p.completionTime
                  << std::setw(18) << p.turnaroundTime
                  << std::setw(15) << p.waitingTime << std::endl;
        totalWT += p.waitingTime;
        totalTAT += p.turnaroundTime;
    }

    std::cout << "---------------------------------------------------------------------------------\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Waiting Time    : " << (n > 0 ? totalWT / n : 0) << std::endl;
    std::cout << "Average Turnaround Time : " << (n > 0 ? totalTAT / n : 0) << std::endl;
    std::cout << "---------------------------------------------------------------------------------\n";
}

// --- SJF (Non-Preemptive) Calculation ---
void calculateSJF_NonPreemptive(std::vector<Process>& processes) {
    int n = processes.size();
    if (n == 0) return;

    int currentTime = 0;
    int completedProcesses = 0;

    while (completedProcesses < n) {
        int shortestJobIndex = -1;
        int minBurstTime = std::numeric_limits<int>::max();

        // Find the available process with the shortest burst time
        for (int i = 0; i < n; ++i) {
            if (!processes[i].isCompleted && processes[i].arrivalTime <= currentTime) {
                if (processes[i].burstTime < minBurstTime) {
                    minBurstTime = processes[i].burstTime;
                    shortestJobIndex = i;
                }
                // Tie-breaking: If burst times are equal, choose the one that arrived earlier
                else if (processes[i].burstTime == minBurstTime) {
                    if (processes[i].arrivalTime < processes[shortestJobIndex].arrivalTime) {
                        shortestJobIndex = i;
                    }
                     // Optional further tie-break: lower ID if arrival times also equal
                    else if (processes[i].arrivalTime == processes[shortestJobIndex].arrivalTime && processes[i].id < processes[shortestJobIndex].id) {
                         shortestJobIndex = i;
                    }
                }
            }
        }

        if (shortestJobIndex == -1) {
            // No process available yet, CPU is idle. Advance time.
            // Find the earliest arrival time of a pending process
            int minArrivalTime = std::numeric_limits<int>::max();
             for(int i=0; i<n; ++i) {
                 if (!processes[i].isCompleted) {
                     minArrivalTime = std::min(minArrivalTime, processes[i].arrivalTime);
                 }
             }
             // If found an arrival time greater than current time, advance time to it.
             if(minArrivalTime > currentTime) {
                 currentTime = minArrivalTime;
             } else { // Should not happen if logic is correct, but ensures progress
                 currentTime++;
             }
        } else {
            // Process the selected shortest job
            Process& currentProcess = processes[shortestJobIndex];

            // Calculate metrics
            // Start time is the current time (since it wasn't preempted)
            currentProcess.completionTime = currentTime + currentProcess.burstTime;
            currentProcess.turnaroundTime = currentProcess.completionTime - currentProcess.arrivalTime;
            currentProcess.waitingTime = currentProcess.turnaroundTime - currentProcess.burstTime;
             if (currentProcess.waitingTime < 0) currentProcess.waitingTime = 0; // Precaution


            // Mark as completed and update current time
            currentProcess.isCompleted = true;
            currentTime = currentProcess.completionTime;
            completedProcesses++;
        }
    }
}

int main() {
    int n;
    std::cout << "--- SJF (Non-Preemptive) Scheduling ---" << std::endl;
    std::cout << "Enter the number of processes: ";
    std::cin >> n;

    if (n <= 0) {
        std::cout << "Number of processes must be positive." << std::endl;
        return 1;
    }

    std::vector<Process> processes(n);
    std::cout << "Enter process details (Arrival Time and Burst Time):\n";
    for (int i = 0; i < n; ++i) {
        processes[i].id = i + 1;
        std::cout << "Process " << processes[i].id << ":\n";
        std::cout << "  Arrival Time: ";
        std::cin >> processes[i].arrivalTime;
         if (processes[i].arrivalTime < 0) {
            std::cout << "Warning: Arrival time cannot be negative. Using 0.\n";
            processes[i].arrivalTime = 0;
        }
        std::cout << "  Burst Time: ";
        std::cin >> processes[i].burstTime;
         if (processes[i].burstTime <= 0) {
             std::cout << "Error: Burst time must be positive. Please re-enter.\n";
             i--; // Re-enter details
             continue;
        }
    }

    calculateSJF_NonPreemptive(processes);
    displayResults(processes);

    return 0;
}
