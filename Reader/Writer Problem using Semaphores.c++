// reader_writer.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <shared_mutex> // Requires C++17
#include <chrono>       // For sleep
#include <random>       // For random sleep times

// Shared resource
int shared_data = 0;
int writer_count = 0; // Optional: track writers for output clarity

// Synchronization primitives
std::shared_mutex rw_mutex; // Allows multiple readers or one writer
std::mutex cout_mutex;     // To protect console output

// Random number generator for sleep times
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(50, 200); // Sleep for 50-200 ms

void reader(int id) {
    for (int i = 0; i < 5; ++i) { // Each reader reads 5 times
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen))); // Simulate time before reading

        // Acquire shared lock (multiple readers allowed)
        std::shared_lock<std::shared_mutex> lock(rw_mutex);

        // --- Critical Section (Reading) ---
        int data_read = shared_data;
        { // Lock cout for printing
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Reader " << id << " read data: " << data_read
                      << " (Writer Count: " << writer_count << ")" << std::endl;
        }
        // --- End Critical Section ---

        // lock is automatically released here (RAII)

        // Simulate time processing data
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen)));
    }
}

void writer(int id) {
    for (int i = 0; i < 3; ++i) { // Each writer writes 3 times
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen))); // Simulate time before writing

        // Acquire unique lock (exclusive access)
        std::unique_lock<std::shared_mutex> lock(rw_mutex);

        // --- Critical Section (Writing) ---
        shared_data++;
        writer_count++; // Track active writer (only 1 possible)
        int data_written = shared_data;
        { // Lock cout for printing
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Writer " << id << " wrote data: " << data_written
                      << " ************ WRITING ************" << std::endl;
        }
        // Simulate writing time
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen) / 2));
        writer_count--;
        // --- End Critical Section ---

        // lock is automatically released here (RAII)

        // Simulate time doing other work
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen)));
    }
}

int main() {
    std::cout << "--- Reader/Writer Problem (using std::shared_mutex) ---" << std::endl;

    const int NUM_READERS = 5;
    const int NUM_WRITERS = 2;

    std::vector<std::thread> threads;

    // Create reader threads
    for (int i = 0; i < NUM_READERS; ++i) {
        threads.emplace_back(reader, i + 1);
    }

    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; ++i) {
        threads.emplace_back(writer, i + 1);
    }

    // Wait for all threads to complete
    for (auto& th : threads) {
        th.join();
    }

    std::cout << "--- All threads finished ---" << std::endl;
    std::cout << "Final shared data value: " << shared_data << std::endl;

    return 0;
}
