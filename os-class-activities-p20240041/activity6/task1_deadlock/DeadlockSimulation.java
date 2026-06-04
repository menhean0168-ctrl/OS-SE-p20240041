import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicBoolean;

class Account {
    String name;
    int balance;
    Semaphore lock = new Semaphore(1);

    Account(String name, int balance) {
        this.name = name;
        this.balance = balance;
    }
}

class Transfer {
    static void transfer(Account from, Account to, int amount, AtomicBoolean done) {
        try {
            System.out.println(Thread.currentThread().getName()
                    + " trying to lock FROM " + from.name);
            from.lock.acquire();
            System.out.println(Thread.currentThread().getName()
                    + " locked FROM " + from.name
                    + " | NOW WAITING for " + to.name + "...");

            Thread.sleep(100); // Sleep so the other thread can grab its lock first

            System.out.println(Thread.currentThread().getName()
                    + " trying to lock TO " + to.name);
            to.lock.acquire(); // <-- This is where deadlock happens
            System.out.println(Thread.currentThread().getName()
                    + " locked TO " + to.name);

            from.balance -= amount;
            to.balance += amount;
            done.set(true);

            System.out.println(Thread.currentThread().getName()
                    + " transfer of $" + amount + " completed");

            to.lock.release();
            from.lock.release();
        } catch (InterruptedException e) {
            System.out.println(Thread.currentThread().getName() + " was interrupted.");
        }
    }
}

public class DeadlockSimulation {
    public static void main(String[] args) throws InterruptedException {
        Account accountA = new Account("Account-A", 1000);
        Account accountB = new Account("Account-B", 1000);

        System.out.println("=== Bank Transfer Deadlock Simulation ===");
        System.out.println("Starting Balance - Account-A: $" + accountA.balance
                + " | Account-B: $" + accountB.balance);
        System.out.println("Starting total: $" + (accountA.balance + accountB.balance));
        System.out.println();

        AtomicBoolean transfer1Done = new AtomicBoolean(false);
        AtomicBoolean transfer2Done = new AtomicBoolean(false);

        // Thread-1: A -> B (locks A first, then tries B)
        Thread t1 = new Thread(() ->
                Transfer.transfer(accountA, accountB, 100, transfer1Done),
                "Thread-1"
        );

        // Thread-2: B -> A (locks B first, then tries A)
        // This creates circular wait with Thread-1
        Thread t2 = new Thread(() ->
                Transfer.transfer(accountB, accountA, 200, transfer2Done),
                "Thread-2"
        );

        t1.start();
        t2.start();

        // Watchdog: check after 2 seconds if transfers are stuck
        Thread watchdog = new Thread(() -> {
            try {
                Thread.sleep(2000);
                if (!transfer1Done.get() || !transfer2Done.get()) {
                    System.out.println();
                    System.out.println("*** Deadlock detected: transactions are stuck ***");
                    System.out.println("Thread-1 is waiting for Account-B");
                    System.out.println("Thread-2 is waiting for Account-A");
                    System.out.println("Neither thread can proceed — circular wait!");
                    System.out.println();
                    System.out.println("Final Account-A: $" + accountA.balance
                            + " (unchanged)");
                    System.out.println("Final Account-B: $" + accountB.balance
                            + " (unchanged)");
                    t1.interrupt();
                    t2.interrupt();
                }
            } catch (InterruptedException e) {
                // watchdog stopped
            }
        });
        watchdog.setDaemon(true);
        watchdog.start();

        t1.join();
        t2.join();
    }
}
