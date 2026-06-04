// Task 2A: Print HELLO - BEFORE Semaphores (Wrong/Random Order)
// Three threads print letters concurrently with no ordering — result is unpredictable.

#include <iostream>
#include <thread>
#include <chrono>

// Process 1 prints H and E
void process1() {
    // No synchronization — runs whenever the OS schedules it
    std::cout << "H";
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // tiny gap exposes race
    std::cout << "E";
    std::cout.flush();
}

// Process 2 prints L and L
void process2() {
    std::cout << "L";
    std::cout.flush();
    std::cout << "L";
    std::cout.flush();
}

// Process 3 prints O
void process3() {
    std::cout << "O";
    std::cout.flush();
}

int main() {
    std::cout << "=== Task 2A: HELLO WITHOUT Semaphores ===\n";
    std::cout << "Running 10 times to show unpredictable ordering:\n\n";

    for (int i = 0; i < 10; i++) {
        std::cout << "Run " << (i+1) << ": ";
        std::cout.flush();

        std::thread t1(process1);
        std::thread t2(process2);
        std::thread t3(process3);

        t1.join();
        t2.join();
        t3.join();

        std::cout << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "\nNotice: letters appear in different orders each run!\n";
    std::cout << "Without semaphores there is no guarantee of HELLO ordering.\n";
    return 0;
}
