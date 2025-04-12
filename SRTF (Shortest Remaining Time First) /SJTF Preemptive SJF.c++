#include <iostream>
#include <vector>
#include <algorithm> // For std::sort, std::min_element
#include <limits>    // For std::numeric_limits
#include <iomanip>   // For formatting output

// Structure to represent a process
struct Process {
    int id;              // Process ID
    int arrivalTime;     // Arrival Time (AT)
    int burstTime;       // Original Burst Time (BT)
    int remainingTime;   // Remaining Burst Time (RT)
    int completionTime;  // Completion Time (CT)
    int turnaroundTime;  // Turnaround Time (TAT)
    int waitingTime;     // Waiting Time (WT)
    int startTime;       // Time process first starts execution (optional for analysis)
    bool isCompleted;    // Flag (alternative to checking remainingTime == 0)

    // Constructor
    Process(int i = 0, int at = 0, int bt = 0)
        : id(i), arrivalTime(at), burstTime(bt), remainingTime(bt),
          completionTime(0), turnaroundTime(0), waitingTime(0), startTime(-1), isCompleted(false) {}
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

    // Optional: Sort by ID for consistent display order
    std::vector<Process> sorted_processes = processes;
    std::sort(sorted_processes.begin(), sorted_processes.end(),
              [](const Process& a, const Process& b){ return a.id < b.id; });


    std::cout << "\n--- SRTF (Preemptive SJF) Schedule Results ---\n";
    std::cout << "---------------------------------------------------------------------------------\n";
    std::cout << std::setw(5) << "PID" << std::setw(15) << "Arrival Time"
              << std::setw(15) << "Burst Time"
              << std::setw(20) << "Completion Time"
              << std::setw(18) << "Turnaround Time"
              << std::setw(15) << "Waiting Time" << std::endl;
    std::cout << "---------------------------------------------------------------------------------\n";

    for (const auto& p : sorted_processes) { // Display sorted by ID
        std::cout << std::setw(5) << p.id
                  << std::setw(15) << p.arrivalTime
                  << std::setw(15) << p.burstTime // Show original burst time
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

// --- SRTF (Preemptive SJF) Calculation ---
void calculateSRTF(std::vector<Process>& processes) {
    int n = processes.size();
    if (n == 0) return;

    int currentTime = 0;
    int completedProcesses = 0;
    int totalBurstTime = 0;
    for(const auto& p : processes) totalBurstTime += p.burstTime;


    while (completedProcesses < n) {
        int shortestJobIndex = -1;
        int minRemainingTime = std::numeric_limits<int>::max();

        // Find the available process with the shortest remaining time
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0) {
                if (processes[i].remainingTime < minRemainingTime) {
                    minRemainingTime = processes[i].remainingTime;
                    shortestJobIndex = i;
                }
                // Tie-breaking: If remaining times are equal, choose the one that arrived earlier
                else if (processes[i].remainingTime == minRemainingTime) {
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
            // No process ready to run, CPU is idle. Advance time.
             int nextArrivalTime = std::numeric_limits<int>::max();
             for(int i=0; i<n; ++i) {
                 if (processes[i].remainingTime > 0) { // If not completed
                     nextArrivalTime = std::min(nextArrivalTime, processes[i].arrivalTime);
                 }
             }
             // Advance only if the next arrival is after the current time
             if(nextArrivalTime > currentTime) {
                 currentTime = nextArrivalTime;
             } else { // Should only happen if all processes are done, but loop condition prevents it
                 currentTime++; // Default advance if something unexpected happens
             }
        } else {
            // Execute the shortest job for one time unit
            Process& currentProcess = processes[shortestJobIndex];

             // Record start time if it's the first time running
             if (currentProcess.startTime == -1) {
                 currentProcess.startTime = currentTime;
             }

            currentProcess.remainingTime--;
            currentTime++; // Advance system time

            // Check if the process completed
            if (currentProcess.remainingTime == 0) {
                currentProcess.completionTime = currentTime;
                currentProcess.turnaroundTime = currentProcess.completionTime - currentProcess.arrivalTime;
                currentProcess.waitingTime = currentProcess.turnaroundTime - currentProcess.burstTime;
                 if (currentProcess.waitingTime < 0) currentProcess.waitingTime = 0; // Precaution

                completedProcesses++;
                currentProcess.isCompleted = true; // Mark completed (optional flag)
            }
        }
    }
}


int main() {
    int n;
    std::cout << "--- SRTF (Preemptive SJF) Scheduling ---" << std::endl;
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
        processes[i].remainingTime = processes[i].burstTime; // Initialize remaining time
        processes[i].startTime = -1; // Initialize start time marker
    }

    calculateSRTF(processes);
    displayResults(processes);

    return 0;
}
