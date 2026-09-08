# EXAM STUDY SHEET 3: ADVANCED SCHEDULER (MLFQS)

## 1. The Core Concept
MLFQS abandons strict, user-assigned priorities and priority donation. Instead, it dynamically calculates priority based on a thread's CPU usage (`recent_cpu`) and its politeness (`nice`). The goal is fair-share scheduling and starvation prevention.

## 2. Fixed-Point Math (17.14)
*   **Why?** The Pintos kernel cannot use the FPU (hardware floating-point unit) because saving/restoring FPU registers on context switches is too slow.
*   **How?** We simulate decimals using bit-shifting. $F = 1 \ll 14$ (16,384).
    *   To convert int to FP: Multiply by $F$.
    *   To multiply two FP numbers: Multiply them, then divide by $F$ (cast to 64-bit first to avoid overflow).

## 3. The MLFQS Heartbeat (Inside `thread_tick`)

### A. Every Tick (10ms) - The Penalty
*   Adds 1 to the running thread's `recent_cpu` (unless it's the `idle_thread`).
*   *Equation:* $recent\_cpu = recent\_cpu + 1$

### B. Every 4 Ticks (40ms) - The Re-Sort
*   Iterates through EVERY thread in `all_list` to recalculate priority.
*   *Equation:* 
    $$priority = 63 - \lfloor \frac{recent\_cpu}{4} \rfloor - (nice \times 2)$$
*   *Exam Note:* Notice how high `recent_cpu` (CPU hogs) and high `nice` (polite threads) lower the priority. The result is clamped between 0 and 63.

### C. Every 100 Ticks (1s) - The Forgiveness (Decay)
*   Updates the global `load_avg` (an exponentially weighted moving average of ready threads).
    $$load\_avg = \left(\frac{59}{60} \times load\_avg\right) + \left(\frac{1}{60} \times ready\_threads\right)$$
*   Iterates through EVERY thread in `all_list` to decay their `recent_cpu`. If `load_avg` is high (congested), decay is slow. If `load_avg` is low (empty), decay is fast.
    $$recent\_cpu = \left( \frac{2 \times load\_avg}{2 \times load\_avg + 1} \right) \times recent\_cpu + nice$$

## 4. Typical Exam Questions to Expect
*   *Q: How does MLFQS prevent thread starvation?* (Because `recent_cpu` decays over time. If a thread is starved, its `recent_cpu` drops to 0, which pushes its calculated priority back up to the maximum, ensuring it eventually runs).
*   *Q: What happens if a thread calls `thread_set_priority()` while MLFQS is active?* (The kernel completely ignores it. MLFQS dictates that threads cannot manually control their own priority).