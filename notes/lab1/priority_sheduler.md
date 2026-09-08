# EXAM STUDY SHEET 2: PRIORITY SCHEDULING & DONATION

## 1. Strict Priority Scheduling
*   **The Rule:** The `THREAD_READY` thread with the highest priority must ALWAYS be the one running on the CPU.
*   **Implementation:** We replaced `list_push_back` with `list_insert_ordered` using a custom priority comparator for the `ready_list`, Semaphore `waiters`, and Condition Variable `waiters`.
*   **The Yield Rule:** If a running thread lowers its own priority, OR wakes up a sleeping thread that has a higher priority than itself, it must immediately call `thread_yield()`.

## 2. Priority Inversion (The Core Problem)
*   **Scenario:** High (H) waits for a lock held by Low (L). Medium (M) is on the ready list. Because M > L, M runs forever. H starves, even though it has the highest priority.
*   **The Solution:** H donates its priority to L. L runs at H's priority, beats M, finishes the critical section, releases the lock, and drops back down, allowing H to proceed.

## 3. The 3 Types of Donation (How you coded them)

### A. Basic Donation
*   Happens in `lock_acquire()`.
*   If the lock is currently held, compare `thread_current()->priority` to `lock->holder->priority`.
*   If current is higher, boost the holder.

### B. Multiple Donation
*   **Scenario:** Thread L holds Lock A and Lock B. It receives a priority 60 donation for A, and a priority 50 donation for B. It releases A. What happens? It should drop to 50, not its base priority!
*   **Implementation:** 
    *   Added `struct list locks_held` to `struct thread`.
    *   In `lock_release()`, the lock is removed from `locks_held`.
    *   Call `update_priority()`, which loops through all *remaining* locks in `locks_held`, looks at the highest waiter on each, and resets the thread's priority to the maximum of those waiters (or its `base_priority` if no one is waiting).

### C. Nested Donation (The Chain)
*   **Scenario:** H waits for Lock 1 (held by M). M waits for Lock 2 (held by L). H's priority must flow all the way to L.
*   **Implementation:** 
    *   Added `struct lock *lock_waiting_on` to `struct thread`. 
    *   In `lock_acquire()`, right before blocking, log the lock: `current->lock_waiting_on = lock`.
    *   Use a `for (int i=0; i<8; i++)` loop to traverse the chain of holders, boosting each one until you hit a thread that isn't waiting on anything (or hit the 8-level depth limit).

## 4. Typical Exam Questions to Expect
*   *Q: Why is priority donation only implemented for Locks, and not Semaphores?* (Because Locks have a strict, identifiable `holder` thread to receive the donation. Semaphores don't).
*   *Q: What is the difference between `base_priority` and `priority`?* (`base_priority` is the thread's native priority; `priority` is its effective priority, which may be temporarily elevated by donations).