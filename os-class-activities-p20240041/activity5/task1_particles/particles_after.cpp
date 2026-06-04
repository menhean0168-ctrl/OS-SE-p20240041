// Task 1B: Particle Pair Buffer - AFTER Semaphores (Correct)
// Uses counting + mutex semaphores to protect the shared buffer.

#include <iostream>
#include <thread>
#include <vector>
#include <deque>
#include <string>
#include <chrono>
#include <atomic>
#include <semaphore>  // C++20

const int BUFFER_CAPACITY = 100; // max particles = 50 pairs

struct Particle {
    int machine_id;
    int pair_id;
    int which; // 1 = P1, 2 = P2
    std::string label() const {
        return "M" + std::to_string(machine_id) + "-" + std::to_string(pair_id) + "-P" + std::to_string(which);
    }
};

// ── Semaphores ──────────────────────────────────────────────────────────────
// empty_pairs: how many pair-slots are free (starts full: 50 pairs)
std::counting_semaphore<50> empty_pairs(50);
// full_pairs: how many complete pairs are ready to consume (starts 0)
std::counting_semaphore<50> full_pairs(0);
// mutex: binary semaphore protecting the buffer and counters
std::binary_semaphore mutex_sem(1);

// Shared state
std::deque<Particle> buffer;
std::atomic<int> produced_pairs(0);
std::atomic<int> packaged_pairs(0);
std::atomic<bool> error_flag(false);
std::string error_message;
std::atomic<int> global_pair_id(0); // unique pair ID across all machines

void producer(int machine_id) {
    while (!error_flag) {
        // Create the pair locally (outside critical section)
        int pid = ++global_pair_id;
        Particle p1{machine_id, pid, 1};
        Particle p2{machine_id, pid, 2};

        // Wait for a free pair-slot in the buffer
        empty_pairs.acquire();
        if (error_flag) return;

        // Enter critical section
        mutex_sem.acquire();

        // Safety check (should never trigger with correct semaphore logic)
        if ((int)buffer.size() + 2 > BUFFER_CAPACITY) {
            error_flag = true;
            error_message = "The producing machine is broken";
            mutex_sem.release();
            full_pairs.release(); // unblock consumer so it can exit
            return;
        }

        buffer.push_back(p1);
        buffer.push_back(p2);
        produced_pairs++;

        mutex_sem.release();

        // Signal that a new complete pair is available
        full_pairs.release();

        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}

void consumer() {
    while (!error_flag) {
        // Wait for a complete pair to be available
        full_pairs.acquire();
        if (error_flag) return;

        // Enter critical section
        mutex_sem.acquire();

        // Safety check
        if (buffer.size() < 2) {
            error_flag = true;
            error_message = "The packaging machine is broken";
            mutex_sem.release();
            return;
        }

        Particle p1 = buffer.front(); buffer.pop_front();
        Particle p2 = buffer.front(); buffer.pop_front();
        int buf_size = (int)buffer.size();

        // Verify pair integrity
        if (p1.machine_id != p2.machine_id || p1.pair_id != p2.pair_id ||
            p1.which != 1 || p2.which != 2) {
            std::cout << "Bad pair: " << p1.label() << " + " << p2.label() << "\n";
            error_flag = true;
            error_message = "Pairs are incorrect";
            mutex_sem.release();
            return;
        }

        packaged_pairs++;
        int prod = produced_pairs.load();
        int pack = packaged_pairs.load();

        mutex_sem.release();

        // Signal that a pair-slot is now free
        empty_pairs.release();

        std::cout << "Packaged: " << p1.label() << " + " << p2.label()
                  << " | Produced pairs: " << prod
                  << " | Packaged pairs: " << pack
                  << " | Buffer particles: " << buf_size << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

int main() {
    std::cout << "=== Task 1B: Particle Buffer WITH Semaphores ===\n";
    std::cout << "Buffer capacity: 100 particles (50 pairs)\n";
    std::cout << "Producers: 4 machines | Consumer: 1 packager\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    const int NUM_PRODUCERS = 4;
    std::vector<std::thread> producers;
    for (int i = 1; i <= NUM_PRODUCERS; i++)
        producers.emplace_back(producer, i);

    std::thread cons(consumer);

    for (auto& t : producers) t.join();
    cons.join();

    if (!error_message.empty())
        std::cout << "\nERROR: " << error_message << "\n";

    return 0;
}
