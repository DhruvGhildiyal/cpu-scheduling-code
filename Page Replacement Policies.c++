// page_replacement.cpp
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <algorithm> // For std::find, std::max_element
#include <limits>    // For std::numeric_limits

// --- FIFO Page Replacement ---
int fifoPageReplacement(const std::vector<int>& pages, int capacity) {
    if (capacity <= 0) return pages.size(); // Every access is a fault if no capacity

    std::unordered_set<int> current_frames; // Pages currently in memory frames
    std::queue<int> fifo_queue;             // Order pages entered frames
    int page_faults = 0;

    std::cout << "\n--- FIFO Simulation ---" << std::endl;
    std::cout << "Ref | Frames" << std::endl;
    std::cout << "----|--------" << std::endl;


    for (int page : pages) {
        std::cout << " " << page << "  | ";
        // Check if page is NOT in frames (page fault)
        if (current_frames.find(page) == current_frames.end()) {
            page_faults++;

            // If frames are full, remove the oldest page (front of queue)
            if (current_frames.size() == capacity) {
                int page_to_remove = fifo_queue.front();
                fifo_queue.pop();
                current_frames.erase(page_to_remove);
                 std::cout << "(Fault - Evict " << page_to_remove << ") ";
            } else {
                 std::cout << "(Fault) ";
            }

            // Add the new page
            current_frames.insert(page);
            fifo_queue.push(page);

        } else {
            // Page hit - no fault
             std::cout << "(Hit)   ";
        }

        // Print current frames (order might not match queue, set is unordered)
         std::cout << "[";
         // Print in FIFO queue order for clarity
         std::queue<int> temp_q = fifo_queue;
         bool first = true;
         while(!temp_q.empty()){
            if(!first) std::cout << ", ";
            std::cout << temp_q.front();
            temp_q.pop();
            first = false;
         }
         std::cout << "]" << std::endl;
    }

    return page_faults;
}

// --- LRU Page Replacement ---
int lruPageReplacement(const std::vector<int>& pages, int capacity) {
     if (capacity <= 0) return pages.size();

    // Store pages currently in frames. Using list to easily move elements.
    // Front = Most Recently Used, Back = Least Recently Used
    std::list<int> frame_list;
    // Map page number to its iterator in the list for O(1) access
    std::unordered_map<int, std::list<int>::iterator> frame_map;
    int page_faults = 0;

    std::cout << "\n--- LRU Simulation ---" << std::endl;
    std::cout << "Ref | Frames (MRU..LRU)" << std::endl;
    std::cout << "----|------------------" << std::endl;

    for (int page : pages) {
         std::cout << " " << page << "  | ";
        // Check if page is NOT in frames (page fault)
        if (frame_map.find(page) == frame_map.end()) {
            page_faults++;

            // If frames are full, remove the LRU page (back of list)
            if (frame_list.size() == capacity) {
                int lru_page = frame_list.back();
                frame_list.pop_back();
                frame_map.erase(lru_page);
                 std::cout << "(Fault - Evict " << lru_page << ") ";
            } else {
                 std::cout << "(Fault) ";
            }

            // Add the new page to the front (MRU)
            frame_list.push_front(page);
            frame_map[page] = frame_list.begin();

        } else {
            // Page hit - Move the accessed page to the front (MRU)
            std::cout << "(Hit)   ";
            // Erase from current position and add to front
            frame_list.erase(frame_map[page]);
            frame_list.push_front(page);
            frame_map[page] = frame_list.begin(); // Update iterator in map
        }

        // Print current frames (in MRU -> LRU order)
        std::cout << "[";
        bool first = true;
        for(int p : frame_list) {
            if(!first) std::cout << ", ";
            std::cout << p;
            first = false;
        }
        std::cout << "]" << std::endl;
    }
    return page_faults;
}

// --- Optimal Page Replacement ---
int optimalPageReplacement(const std::vector<int>& pages, int capacity) {
    if (capacity <= 0) return pages.size();

    std::unordered_set<int> current_frames; // Can use set or vector
    std::vector<int> frame_vector; // To keep track of order for printing
    int page_faults = 0;

    std::cout << "\n--- Optimal Simulation ---" << std::endl;
    std::cout << "Ref | Frames" << std::endl;
    std::cout << "----|--------" << std::endl;

    for (size_t i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        std::cout << " " << page << "  | ";

        // Check if page is NOT in frames (page fault)
        if (current_frames.find(page) == current_frames.end()) {
            page_faults++;

            // If frames are full, find the page to evict
            if (current_frames.size() == capacity) {
                int page_to_evict = -1;
                int farthest_use = -1; // Index of the farthest future use

                // Iterate through pages currently in frames
                for (int frame_page : frame_vector) {
                    int next_use = std::numeric_limits<int>::max(); // Assume never used again initially
                    // Find the next occurrence of frame_page in the remaining reference string
                    for (size_t j = i + 1; j < pages.size(); ++j) {
                        if (pages[j] == frame_page) {
                            next_use = j;
                            break;
                        }
                    }

                    // If this page's next use is farther than the current farthest,
                    // or if this page is never used again (next_use is max), it's a candidate
                    if (next_use > farthest_use) {
                        farthest_use = next_use;
                        page_to_evict = frame_page;
                    }
                     // If multiple pages are never used again, evicting any of them is optimal.
                     // This implementation picks the last one checked among those never used again.
                }

                 std::cout << "(Fault - Evict " << page_to_evict << ") ";
                current_frames.erase(page_to_evict);
                // Remove from vector too (find and erase)
                 frame_vector.erase(std::remove(frame_vector.begin(), frame_vector.end(), page_to_evict), frame_vector.end());

            } else {
                std::cout << "(Fault) ";
            }

            // Add the new page
            current_frames.insert(page);
             frame_vector.push_back(page); // Add to vector for tracking

        } else {
             std::cout << "(Hit)   ";
            // Page hit - no fault, no change in frames needed for Optimal
        }
         // Print current frames
        std::cout << "[";
        bool first = true;
        for(int p : frame_vector) { // Print from vector to show content
            if(!first) std::cout << ", ";
            std::cout << p;
            first = false;
        }
        std::cout << "]" << std::endl;
    }
    return page_faults;
}

int main() {
    // Example Page Reference String
    std::vector<int> page_references = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    // std::vector<int> page_references = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

    int frame_capacity = 4; // Number of available frames in memory

    std::cout << "Page Reference String: ";
    for(int p : page_references) std::cout << p << " ";
    std::cout << "\nNumber of Frames: " << frame_capacity << std::endl;

    int fifo_faults = fifoPageReplacement(page_references, frame_capacity);
    std::cout << "Total Page Faults (FIFO): " << fifo_faults << std::endl;

    int lru_faults = lruPageReplacement(page_references, frame_capacity);
    std::cout << "Total Page Faults (LRU): " << lru_faults << std::endl;

    int optimal_faults = optimalPageReplacement(page_references, frame_capacity);
    std::cout << "Total Page Faults (Optimal): " << optimal_faults << std::endl;


    return 0;
}
