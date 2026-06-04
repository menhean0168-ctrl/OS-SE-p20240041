import java.util.concurrent.Semaphore;

class AccountFixed {
    String name;
    int balance;

    AccountFixed(String name, int balance) {
        this.name = name;
        this.balance = balance;
    }
}

class SafeTransfer {
    // Single global mutex — only one transfer runs at a time
    static Semaphore mutex = new Semaphore(1);

    static void transfer(AccountFixed from, AccountFixed to, int amount) {
        try {
            mutex.acquire(); // Lock the entire transfer operation
            try {
                System.out.println(Thread.currentThread().getName()
                        + " acquired mutex — starting transfer of $" + amount
                        + " from " + from.name + " to " + to.name);

                Thread.sleep(100); // Simulate processing time

                from.balance -= amount;
                to.balance += amount;

                System.out.println(Thread.currentThread().getName()
                        + " transferred $" + amount
                        + " from " + from.name + " to " + to.name
                        + " | " + from.name + ": $" + from.balance
                        + " | " + to.name + ": $" + to.balance);
            } finally {
                mutex.release(); // Always release, even if an error occurs
                System.out.println(Thread.currentThread().getName()
                        + " released mutex");
            }
        } catch (InterruptedException e) {
            System.out.println(Thread.currentThread().getName() + " was interrupted.");
        }
    }
}

public class DeadlockFixed {
    public static void main(String[] args) throws InterruptedException {
        AccountFixed accountA = new AccountFixed("Account-A", 1000);
        AccountFixed accountB = new AccountFixed("Account-B", 1000);

        int startingTotal = accountA.balance + accountB.balance;

        System.out.println("=== Bank Transfer Deadlock Prevention ===");
        System.out.println("Starting Balance - Account-A: $" + accountA.balance
                + " | Account-B: $" + accountB.balance);
        System.out.println("Starting total: $" + startingTotal);
        System.out.println("Strategy: single semaphore mutex (initialized to 1)");
        System.out.println();

        // Same two transfers as Task 1 — but now protected by one shared mutex
        Thread t1 = new Thread(() ->
                SafeTransfer.transfer(accountA, accountB, 100),
                "Thread-1"
        );

        Thread t2 = new Thread(() ->
                SafeTransfer.transfer(accountB, accountA, 200),
                "Thread-2"
        );

        t1.start();
        t2.start();

        t1.join();
        t2.join();

        int finalTotal = accountA.balance + accountB.balance;

        System.out.println();
        System.out.println("=== Results ===");
        System.out.println("Final Account-A: $" + accountA.balance);
        System.out.println("Final Account-B: $" + accountB.balance);
        System.out.println("Final total: $" + finalTotal);
        System.out.println("Starting total: $" + startingTotal);

        if (finalTotal == startingTotal) {
            System.out.println("Balance check PASSED — no money created or lost");
        } else {
            System.out.println("Balance check FAILED — something went wrong");
        }

        System.out.println("No deadlock occurred");
    }
}
