// Task 1A: Particle Pair Buffer - BEFORE Semaphores (Unsafe)
// Demonstrates broken behavior without synchronization.

#include <iostream>
#include <thread>
#include <vector>
#include <deque>
#include <string>
#include <chrono>
#include <atomic>
#include <sstream>

const int BUFFER_CAPACITY = 100; // max particles

struct Particle {
    int machine_id;
    int pair_id;
    int which; // 1 = P1, 2 = P2
    std::string label() const {
        return "M" + std::to_string(machine_id) + "-" + std::to_string(pair_id) + "-P" + std::to_string(which);
    }
};

// Shared state — NO protection
std::deque<Particle> buffer;
std::atomic<int> produced_pairs(0);
std::atomic<int> packaged_pairs(0);
std::atomic<bool> error_flag(false);
std::string error_message;

// Next pair ID per machine (unprotected)
int pair_counter[5] = {0,0,0,0,0};

void producer(int machine_id) {
    while (!error_flag) {
        int pid = ++pair_counter[machine_id]; // race condition here

        Particle p1{machine_id, pid, 1};
        Particle p2{machine_id, pid, 2};

        // No mutex — multiple threads touch buffer simultaneously
        if ((int)buffer.size() + 2 > BUFFER_CAPACITY) {
            error_flag = true;
            error_message = "The producing machine is broken";
            return;
        }

        // Intentional gap — another thread can sneak in between these two pushes
        buffer.push_back(p1);
        std::this_thread::sleep_for(std::chrono::microseconds(10)); // expose the race
        buffer.push_back(p2);

        produced_pairs++;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void consumer() {
    while (!error_flag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));

        if (buffer.size() < 2) {
            if (buffer.empty()) {
                error_flag = true;
                error_message = "The packaging machine is broken";
                return;
            }
            continue;
        }

        Particle p1 = buffer.front(); buffer.pop_front();
        Particle p2 = buffer.front(); buffer.pop_front();

        // Verify pair integrity
        if (p1.machine_id != p2.machine_id || p1.pair_id != p2.pair_id) {
            std::cout << "Packaging: " << p1.label() << " + " << p2.label() << " <-- MISMATCH!\n";
            error_flag = true;
            error_message = "Pairs are incorrect";
            return;
        }

        packaged_pairs++;
        std::cout << "Packaged: " << p1.label() << " + " << p2.label()
                  << " | Produced: " << produced_pairs
                  << " | Packaged: " << packaged_pairs
                  << " | Buffer: " << buffer.size() << "\n";
    }
}

int main() {
    std::cout << "=== Task 1A: Particle Buffer WITHOUT Semaphores ===\n";
    std::cout << "Expect broken behavior (mismatched pairs, crashes, etc.)\n\n";

    std::vector<std::thread> producers;
    for (int i = 1; i <= 3; i++)
        producers.emplace_back(producer, i);

    std::thread cons(consumer);

    for (auto& t : producers) t.join();
    cons.join();

    if (!error_message.empty())
        std::cout << "\nERROR: " << error_message << "\n";

    return 0;
}
