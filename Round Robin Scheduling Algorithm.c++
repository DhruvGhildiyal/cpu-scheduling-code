// round_robin.cpp
#include <iostream>
#include <vector>
#include <queue>
#include <numeric>   // For accumulate
#include <iomanip>   // For formatting output
#include <algorithm> // For min

struct ProcessRR {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_burst_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    bool in_queue; // To prevent adding the same process multiple times if it arrives while checked

    ProcessRR(int id, int at, int bt) :
        pid(id), arrival_time(at), burst_time(bt), remaining_burst_time(bt),
        completion_time(0), turnaround_time(0), waiting_time(0), in_queue(false) {}
};

void roundRobinScheduling(std::vector<ProcessRR>& processes, int quantum) {
    int n = processes.size();
    if (n == 0) return;

    std::queue<int> ready_queue; // Stores indices of processes in the processes vector
    int current_time = 0;
    int completed_processes = 0;
    int current_process_index = 0; // To track which process to check for arrival

    std::cout << "\n--- Round Robin Scheduling (Quantum = " << quantum << ") ---" << std::endl;
    std::cout << "Execution Trace (PID | Time):" << std::endl;

    // Initial check for processes arriving at time 0
    while(current_process_index < n && processes[current_process_index].arrival_time <= current_time) {
        if (!processes[current_process_index].in_queue) {
             ready_queue.push(current_process_index);
             processes[current_process_index].in_queue = true;
        }
        current_process_index++;
    }

    while (completed_processes < n) {
        if (ready_queue.empty()) {
            // If queue is empty, advance time to the next arrival
            int next_arrival_time = -1;
            for(int i = 0; i < n; ++i) {
                if(processes[i].remaining_burst_time > 0 && !processes[i].in_queue) {
                     if(next_arrival_time == -1 || processes[i].arrival_time < next_arrival_time) {
                        next_arrival_time = processes[i].arrival_time;
                     }
                }
            }
             if (next_arrival_time != -1) {
                 std::cout << "CPU Idle | " << current_time << " -> " << next_arrival_time << std::endl;
                 current_time = next_arrival_time;
             } else {
                 // Should not happen if completed_processes < n, but as a safeguard
                 break;
             }

            // Add processes that arrived by the new current_time
             current_process_index = 0; // Reset check index
             while(current_process_index < n) {
                 if(processes[current_process_index].remaining_burst_time > 0 &&
                    processes[current_process_index].arrival_time <= current_time &&
                    !processes[current_process_index].in_queue)
                 {
                      ready_queue.push(current_process_index);
                      processes[current_process_index].in_queue = true;
                 }
                 current_process_index++;
             }

             if (ready_queue.empty()) continue; // Still empty? Wait more.
        }

        int idx = ready_queue.front();
        ready_queue.pop();
        processes[idx].in_queue = false; // It's out of the queue now

        int time_slice = std::min(quantum, processes[idx].remaining_burst_time);

        std::cout << "P" << processes[idx].pid << "       | " << current_time << " -> ";

        processes[idx].remaining_burst_time -= time_slice;
        current_time += time_slice;
         std::cout << current_time << (processes[idx].remaining_burst_time == 0 ? " (Finished)" : "") << std::endl;


        // Check for new arrivals during the execution of the current process
        int check_arrival_idx = 0;
         while(check_arrival_idx < n) {
             if(processes[check_arrival_idx].remaining_burst_time > 0 && // Only if not finished
                processes[check_arrival_idx].arrival_time <= current_time &&
                !processes[check_arrival_idx].in_queue && // Only if not already in queue
                check_arrival_idx != idx) // Don't add self if it was just processed
                {
                   // A simple check if it's already in the queue before adding
                   bool already_in_queue = false;
                   std::queue<int> temp_q = ready_queue;
                   while(!temp_q.empty()){
                       if(temp_q.front() == check_arrival_idx) {
                           already_in_queue = true;
                           break;
                       }
                       temp_q.pop();
                   }
                   if(!already_in_queue){
                       ready_queue.push(check_arrival_idx);
                       processes[check_arrival_idx].in_queue = true;
                   }
             }
             check_arrival_idx++;
         }


        if (processes[idx].remaining_burst_time == 0) {
            // Process finished
            completed_processes++;
            processes[idx].completion_time = current_time;
            processes[idx].turnaround_time = processes[idx].completion_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
        } else {
            // Process not finished, add back to the queue
            // Check if it should be added back now or after new arrivals
            ready_queue.push(idx);
            processes[idx].in_queue = true;
        }
    }

     // --- Display Results ---
    std::cout << "\n--- Final Results ---" << std::endl;
    std::cout << std::left << std::setw(5) << "PID"
              << std::setw(15) << "Arrival Time"
              << std::setw(15) << "Burst Time"
              << std::setw(18) << "Completion Time"
              << std::setw(17) << "Waiting Time"
              << std::setw(18) << "Turnaround Time" << std::endl;
    std::cout << std::string(90, '-') << std::endl; // Separator line

    double total_waiting_time = 0;
    double total_turnaround_time = 0;

    // Sort by PID for final display (optional)
    std::sort(processes.begin(), processes.end(), [](const ProcessRR& a, const ProcessRR& b){
        return a.pid < b.pid;
    });


    for (const auto& p : processes) {
        std::cout << std::left << std::setw(5) << p.pid
                  << std::setw(15) << p.arrival_time
                  << std::setw(15) << p.burst_time
                  << std::setw(18) << p.completion_time
                  << std::setw(17) << p.waiting_time
                  << std::setw(18) << p.turnaround_time << std::endl;
        total_waiting_time += p.waiting_time;
        total_turnaround_time += p.turnaround_time;
    }
    std::cout << std::string(90, '-') << std::endl; // Separator line

    std::cout << std::fixed << std::setprecision(2); // Format averages
    std::cout << "\nAverage Waiting Time    : " << (total_waiting_time / n) << std::endl;
    std::cout << "Average Turnaround Time : " << (total_turnaround_time / n) << std::endl;
}


int main() {
    // Example Processes: (PID, Arrival Time, Burst Time)
    std::vector<ProcessRR> processes;
    processes.emplace_back(1, 0, 5);
    processes.emplace_back(2, 1, 4);
    processes.emplace_back(3, 2, 2);
    processes.emplace_back(4, 4, 1);

    // Sort processes by arrival time initially
    std::sort(processes.begin(), processes.end(), [](const ProcessRR& a, const ProcessRR& b) {
        return a.arrival_time < b.arrival_time;
    });


    int time_quantum = 2;
    roundRobinScheduling(processes, time_quantum);

    return 0;
}
