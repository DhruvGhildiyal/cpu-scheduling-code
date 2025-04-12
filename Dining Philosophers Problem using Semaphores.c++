// dining_philosophers.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <random>
#include <string>

const int NUM_PHILOSOPHERS = 5;

// Mutexes representing chopsticks
std::vector<std::mutex> chopsticks(NUM_PHILOSOPHERS);
std::mutex cout_mutex; // For synchronized output

// Random number generator for thinking/eating times
std::random_device rd_dp;
std::mt19937 gen_dp(rd_dp());
std::uniform_int_distribution<> distrib_dp(100, 500); // 100-500 ms

void philosopher(int id) {
    int left_chopstick = id;
    int right_chopstick = (id + 1) % NUM_PHILOSOPHERS;

    // Asymmetric solution to prevent deadlock: last philosopher picks up right first
    if (id == NUM_PHILOSOPHERS - 1) {
        std::swap(left_chopstick, right_chopstick);
    }

    for (int i = 0; i < 3; ++i) { // Each philosopher eats 3 times
        // --- Thinking ---
        {
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Philosopher " << id << " is thinking." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib_dp(gen_dp)));

        // --- Hungry - Try to pick up chopsticks ---
        {
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Philosopher " << id << " is hungry, wants chopsticks "
                      << left_chopstick << " and " << right_chopstick << "." << std::endl;
        }

        // Acquire locks (chopsticks) using std::lock to prevent deadlock during acquisition
        // std::lock locks multiple mutexes atomically
        std::lock(chopsticks[left_chopstick], chopsticks[right_chopstick]);

        // Adopt the locks once acquired by std::lock
        std::lock_guard<std::mutex> lock_left(chopsticks[left_chopstick], std::adopt_lock);
        std::lock_guard<std::mutex> lock_right(chopsticks[right_chopstick], std::adopt_lock);


        // --- Eating ---
        {
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Philosopher " << id << " picked up chopsticks "
                      << left_chopstick << " and " << right_chopstick << ". Eating..." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib_dp(gen_dp)));

        // --- Finished Eating - Put down chopsticks ---
        {
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Philosopher " << id << " finished eating, putting down chopsticks "
                      << left_chopstick << " and " << right_chopstick << "." << std::endl;
        }
        // Locks are automatically released by lock_guard's destructor
    }

    {
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << "Philosopher " << id << " is done." << std::endl;
    }
}

int main() {
    std::cout << "--- Dining Philosophers Problem (using std::mutex) ---" << std::endl;
    std::cout << "Using std::lock for deadlock prevention during acquisition." << std::endl;

    std::vector<std::thread> philosophers_threads;

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers_threads.emplace_back(philosopher, i);
    }

    for (auto& th : philosophers_threads) {
        th.join();
    }

    std::cout << "--- All philosophers have finished ---" << std::endl;

    return 0;
}
