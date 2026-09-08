
# TASK 1: ALARM CLOCK (Removing Busy Waiting)

## The Core Problem
The original `timer_sleep()` used a "busy wait." It spun in a `while` loop, constantly calling `thread_yield()` to give up the CPU, but staying in the `ready_list`. This wasted CPU cycles just checking the time.

## How We Fixed It
We forced sleeping threads to completely leave the scheduling queue and rely on the hardware timer to wake them up.

## Key Functions & Modifications
* **`struct thread` (thread.h):** Added `int64_t wakeup_tick` to store the exact time the thread should wake up.
* **`sleep_list` (timer.c):** Created a global list to hold sleeping threads, kept sorted by `wakeup_tick` so the earliest waker is always at the front.
* **`timer_sleep(ticks)`:** 
  1. Calculated the target wake time.
  2. Called `intr_disable()` to turn off hardware interrupts safely.
  3. Inserted the thread into `sleep_list` using `list_insert_ordered()`.
  4. Called `thread_block()` to change state to `THREAD_BLOCKED` (taking it off the CPU entirely).
  5. Restored interrupts.
* **`timer_interrupt()`:** The hardware heartbeat (fires 100x/sec). 
  * Checks the front of `sleep_list`. 
  * If the front thread's `wakeup_tick` $\le$ current ticks, it removes it from the list and calls `thread_unblock()` to put it back in the `ready_list`. 
  * If not, it hits `break;` immediately (O(1) efficiency).

  # TASK 2: PRIORITY SCHEDULING & DONATION

## The Core Problem
Pintos originally used Round-Robin. We needed strict priority (highest priority always runs) and a fix for **Priority Inversion** (where a High priority thread is blocked by a Low priority thread holding a lock, while a Medium thread hogs the CPU).

## How We Fixed It
We enforced strict ordering on all lists and implemented a system where blocked threads lend their priority to the thread blocking them.

## Key Concepts & Implementation
* **Strict Scheduling:** We replaced `list_push_back` with `list_insert_ordered` (using a custom `thread_compare_priority` function) for the `ready_list`, `sema->waiters`, and `cond->waiters`.
* **The Yield Rule:** If a thread ever lowers its priority, or wakes up a thread with a higher priority, it must immediately call `thread_yield()`.
* **Basic Donation (`lock_acquire`):** If a lock is held, the requesting thread sets the holder's priority to its own (if higher) before blocking.
* **Multiple Donation (`locks_held`):** A thread can hold multiple locks. 
  * Added a `locks_held` list to `struct thread`. 
  * In `lock_release()`, the lock is removed from the inventory. We then call `update_priority()` to look at all remaining locks and revert the thread to the highest priority waiting on any of them (or back to its original `base_priority`).
* **Nested Donation (`lock_waiting_on`):** H waits for M, M waits for L. 
  * Added `lock_waiting_on` to `struct thread`. 
  * In `lock_acquire()`, a `for` loop traces these pointers up to 8 levels deep, boosting every thread in the chain.


  # TASK 3: ADVANCED SCHEDULER (MLFQS)

## The Core Problem
Strict priority starves low-priority threads completely. MLFQS mathematically adjusts priorities dynamically to balance CPU hogs and background tasks, ignoring user-set priorities and priority donation entirely.

## The Math (17.14 Fixed-Point)
The kernel doesn't support floating-point (FPU) hardware. We faked decimals by shifting 32-bit integers left by 14 bits: $F = 1 \ll 14$. 
* Adding/Subtracting: Direct operation.
* Multiplication: Cast to `int64_t`, multiply, then divide by $F$ to fix the scale.
* Division: Cast to `int64_t`, multiply numerator by $F$, then divide.

## The 3 Key Metrics
1. **`nice` (Integer, -20 to +20):** How polite a thread is. Positive = polite (yields CPU), Negative = greedy.
2. **`recent_cpu` (Fixed-Point):** Tracks how much CPU the thread has used recently.
3. **`load_avg` (Fixed-Point):** Global moving average of threads ready to run.

## The Heartbeat (`thread_tick`)
* **Every 1 Tick (10ms):** 
  $$recent\_cpu = recent\_cpu + 1$$ *(For the running thread only).*
* **Every 4 Ticks (40ms):** Recalculates priority for EVERY thread.
  $$priority = 63 - \lfloor \frac{recent\_cpu}{4} \rfloor - (nice \times 2)$$
* **Every 100 Ticks (1s):** Updates global load and decays everyone's CPU penalty.
  $$load\_avg = \left(\frac{59}{60} \times load\_avg\right) + \left(\frac{1}{60} \times ready\_threads\right)$$
  $$recent\_cpu = \left( \frac{2 \times load\_avg}{2 \times load\_avg + 1} \right) \times recent\_cpu + nice$$