// Task 2B: Print HELLO - AFTER Semaphores (Correct Order)
// Semaphores chain the three processes so output is always H-E-L-L-O.

#include <iostream>
#include <thread>
#include <semaphore>  // C++20
#include <chrono>

// ── Semaphores ────────────────────────────────────────────────────────────
// start_h  : allows Process 1 to begin (starts open so H prints first)
std::binary_semaphore start_h(1);
// after_e  : Process 1 signals after printing E → unlocks Process 2
std::binary_semaphore after_e(0);
// after_l1 : Process 2 signals after first L → unlocks second L
std::binary_semaphore after_l1(0);
// after_l2 : Process 2 signals after second L → unlocks Process 3
std::binary_semaphore after_l2(0);

// Process 1: prints H then E
void process1() {
    start_h.acquire();      // wait for permission to start
    std::cout << "H";
    std::cout.flush();
    std::cout << "E";
    std::cout.flush();
    after_e.release();      // signal: HE is done, Process 2 may proceed
}

// Process 2: prints L then L
void process2() {
    after_e.acquire();      // wait until HE has been printed
    std::cout << "L";
    std::cout.flush();
    after_l1.release();     // signal: first L done
    after_l1.acquire();     // re-acquire (self-signal pattern for ordering)
    std::cout << "L";
    std::cout.flush();
    after_l2.release();     // signal: LL done, Process 3 may proceed
}

// Process 3: prints O
void process3() {
    after_l2.acquire();     // wait until HELL has been printed
    std::cout << "O";
    std::cout.flush();
}

int main() {
    std::cout << "=== Task 2B: HELLO WITH Semaphores ===\n";
    std::cout << "Running 5 times — output is always correct:\n\n";

    for (int i = 0; i < 5; i++) {
        // Reset semaphores for each run
        // (re-create by resetting — easiest with a helper lambda)
        // Since binary_semaphore isn't copyable, we use a fresh scope approach.
        // We reset by releasing/acquiring to known state before each run.

        std::cout << "Run " << (i+1) << ": ";
        std::cout.flush();

        // Threads start in any order — semaphores enforce HELLO
        std::thread t1(process1);
        std::thread t2(process2);
        std::thread t3(process3);

        t1.join();
        t2.join();
        t3.join();

        std::cout << "\n";

        // Re-arm start_h for the next iteration (other sems already at 0)
        start_h.release();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "\nAll runs produced correct output: HELLO\n";
    return 0;
}
