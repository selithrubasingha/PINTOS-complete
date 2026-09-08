
# EXAM STUDY SHEET 1: ALARM CLOCK & SLEEP MECHANICS

## 1. The Core Problem: Busy Waiting
*   **What it was:** The original `timer_sleep()` used a `while` loop to check if enough time had passed, repeatedly calling `thread_yield()`. 
*   **Why it’s bad:** The thread remained in the `THREAD_READY` state and the `ready_list`. The scheduler kept putting it on the CPU just for it to realize it was still supposed to be sleeping, wasting CPU cycles and draining battery.

## 2. The Solution: Blocking and Interrupt Wakeups
Instead of the thread polling the clock, the OS puts the thread into a deep sleep (`THREAD_BLOCKED`) and uses the hardware timer interrupt to wake it up.

## 3. Key Data Structures
*   `int64_t wakeup_tick` *(in `struct thread`)*: Stores the exact future OS tick when this specific thread is allowed to wake up.
*   `sleep_list` *(global in `timer.c`)*: A dedicated queue holding only sleeping threads, kept completely separate from the scheduler's `ready_list`.

## 4. Step-by-Step Function Breakdown

### `timer_sleep(int64_t ticks)`
1.  **Calculate Target Time:** `cur->wakeup_tick = timer_ticks() + ticks;`
2.  **Disable Interrupts:** `intr_disable();`
    *   *Exam Note:* Why? Because `timer_interrupt()` also accesses the `sleep_list`. If a hardware tick fires while a kernel thread is halfway through inserting itself into the list, memory gets corrupted (race condition).
3.  **Insert Ordered:** `list_insert_ordered(&sleep_list, ...)`
    *   *Exam Note:* Why ordered? So the thread that needs to wake up earliest is always at the absolute front (head) of the list.
4.  **Block:** `thread_block();` takes the thread off the CPU, changes state to `THREAD_BLOCKED`, and calls `schedule()`.
5.  **Restore Interrupts:** `intr_set_level(old_level);`

### `timer_interrupt()` (The Hardware Heartbeat)
1.  Fires every 10 milliseconds (100 times a second).
2.  Looks at the head of `sleep_list`.
3.  Checks if `t->wakeup_tick <= ticks`. 
4.  If yes $\rightarrow$ `list_remove(e)` and `thread_unblock(t)`.
5.  If no $\rightarrow$ `break;`. 
    *   *Exam Note:* This `break;` is the key to O(1) efficiency. Because the list is sorted, if the first thread isn't ready to wake up, nobody else behind it is ready either.

## 5. Typical Exam Questions to Expect
*   *Q: What is the difference between `thread_yield()` and `thread_block()`?* (Yield goes to ready_list and stays eligible for CPU; Block leaves ready_list and is ignored until explicitly unblocked).
*   *Q: Why must interrupts be disabled inside `timer_sleep`?* (To prevent race conditions on the shared `sleep_list`).
