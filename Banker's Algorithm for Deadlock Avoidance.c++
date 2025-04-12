// bankers_algorithm.cpp
#include <iostream>
#include <vector>
#include <numeric> // For std::accumulate, std::all_of

// Function to check if the system is in a safe state
bool isSafe(int num_processes, int num_resources,
            const std::vector<int>& available,
            const std::vector<std::vector<int>>& max_need,
            const std::vector<std::vector<int>>& allocation,
            std::vector<int>& safe_sequence) // Output parameter for the sequence
{
    std::vector<int> work = available;
    std::vector<bool> finish(num_processes, false);
    std::vector<std::vector<int>> need(num_processes, std::vector<int>(num_resources));

    // Calculate the Need matrix
    for (int i = 0; i < num_processes; ++i) {
        for (int j = 0; j < num_resources; ++j) {
            need[i][j] = max_need[i][j] - allocation[i][j];
            if (need[i][j] < 0) { // Should not happen if input is valid
                std::cerr << "Error: Allocation exceeds max need for P" << i << ", R" << j << std::endl;
                return false;
            }
        }
    }

    safe_sequence.clear();
    int count = 0;
    while (count < num_processes) {
        bool found = false;
        for (int i = 0; i < num_processes; ++i) {
            if (!finish[i]) {
                // Check if Need[i] <= Work
                bool possible = true;
                for (int j = 0; j < num_resources; ++j) {
                    if (need[i][j] > work[j]) {
                        possible = false;
                        break;
                    }
                }

                if (possible) {
                    // Simulate allocation completion
                    for (int j = 0; j < num_resources; ++j) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    safe_sequence.push_back(i); // Add process to safe sequence
                    found = true;
                    count++;
                    // No need to break here, continue checking other processes
                    // This ensures we find *a* safe sequence if one exists
                }
            }
        }
        // If no process could be found in this pass, the system is unsafe
        if (!found) {
             // Check if all processes actually finished already
             bool all_finished = true;
             for(int k=0; k<num_processes; ++k) {
                 if (!finish[k]) {
                     all_finished = false;
                     break;
                 }
             }
             if (all_finished) break; // Normal exit

            safe_sequence.clear(); // Clear partial sequence if unsafe
            return false;
        }
    }

    // If we reach here, all processes are finished
    return true;
}

// Function to handle a resource request from a process
bool requestResources(int process_id, const std::vector<int>& request,
                      int num_processes, int num_resources,
                      std::vector<int>& available,
                      const std::vector<std::vector<int>>& max_need,
                      std::vector<std::vector<int>>& allocation)
{
    std::cout << "\n> P" << process_id << " requests: ";
    for(int r : request) std::cout << r << " ";
    std::cout << std::endl;

    // 1. Check if request <= need
    std::vector<int> need(num_resources);
     for (int j = 0; j < num_resources; ++j) {
        need[j] = max_need[process_id][j] - allocation[process_id][j];
        if (request[j] > need[j]) {
            std::cout << "  Error: Request exceeds maximum claim (Need[" << j << "]=" << need[j] <<"). Request denied." << std::endl;
            return false;
        }
    }

    // 2. Check if request <= available
    for (int j = 0; j < num_resources; ++j) {
        if (request[j] > available[j]) {
            std::cout << "  Error: Resources not available (Available[" << j << "]=" << available[j] << "). Process must wait." << std::endl;
            // In a real system, the process would block here. We just deny for now.
            return false;
        }
    }

    // 3. Pretend to allocate resources
    std::vector<int> temp_available = available;
    std::vector<std::vector<int>> temp_allocation = allocation;

    for (int j = 0; j < num_resources; ++j) {
        temp_available[j] -= request[j];
        temp_allocation[process_id][j] += request[j];
        // Need also changes implicitly (Need = Max - Allocation)
    }

    // 4. Check if the new state is safe
    std::vector<int> safe_sequence;
    if (isSafe(num_processes, num_resources, temp_available, max_need, temp_allocation, safe_sequence)) {
        std::cout << "  Request granted. System remains safe." << std::endl;
        std::cout << "  (A possible safe sequence: ";
        for(size_t i=0; i < safe_sequence.size(); ++i) std::cout << "P" << safe_sequence[i] << (i == safe_sequence.size() - 1 ? "" : " -> ");
        std::cout << ")" << std::endl;

        // Update the actual state
        available = temp_available;
        allocation = temp_allocation;
        return true;
    } else {
        std::cout << "  Request denied. Allocating would lead to an unsafe state." << std::endl;
        // Restore original state (implicitly done as we used temporary copies)
        return false;
    }
}


int main() {
    // Example from Silberschatz, Galvin, Gagne OS Concepts book
    const int P = 5; // Number of processes
    const int R = 3; // Number of resource types

    // Allocation Matrix
    std::vector<std::vector<int>> allocation = {
        {0, 1, 0}, // P0
        {2, 0, 0}, // P1
        {3, 0, 2}, // P2
        {2, 1, 1}, // P3
        {0, 0, 2}  // P4
    };

    // Max Need Matrix
    std::vector<std::vector<int>> max_need = {
        {7, 5, 3}, // P0
        {3, 2, 2}, // P1
        {9, 0, 2}, // P2
        {2, 2, 2}, // P3
        {4, 3, 3}  // P4
    };

    // Available Resources Vector
    std::vector<int> available = {3, 3, 2};

    std::cout << "--- Banker's Algorithm ---" << std::endl;
    std::cout << "Initial State:" << std::endl;
    std::cout << "Available: "; for(int x : available) std::cout << x << " "; std::cout << std::endl;
    // (Optionally print Allocation and Max Need matrices)

    // Check initial state safety
    std::vector<int> initial_safe_sequence;
    if (isSafe(P, R, available, max_need, allocation, initial_safe_sequence)) {
        std::cout << "Initial state is SAFE." << std::endl;
        std::cout << "Safe sequence: ";
        for(size_t i=0; i < initial_safe_sequence.size(); ++i) std::cout << "P" << initial_safe_sequence[i] << (i == initial_safe_sequence.size() - 1 ? "" : " -> ");
        std::cout << std::endl;
    } else {
        std::cout << "Initial state is UNSAFE." << std::endl;
        // If the initial state is unsafe, requests generally shouldn't proceed
        // or the system needs correction.
    }

    // --- Simulate some requests ---

    // Request 1: P1 requests (1, 0, 2)
    std::vector<int> req1 = {1, 0, 2};
    requestResources(1, req1, P, R, available, max_need, allocation);

    // Request 2: P4 requests (3, 3, 0)
    std::vector<int> req2 = {3, 3, 0};
    requestResources(4, req2, P, R, available, max_need, allocation); // Should be denied initially if P1's request was granted

    // Request 3: P0 requests (0, 2, 0)
     std::vector<int> req3 = {0, 2, 0};
    requestResources(0, req3, P, R, available, max_need, allocation);

    return 0;
}
