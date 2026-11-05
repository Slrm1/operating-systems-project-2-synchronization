# Operating Systems Project 2 — Synchronization

Howard University Operating Systems project demonstrating synchronization across both processes and threads.

## Results

- Part 1 — Process synchronization using semaphores: **100/100**
- Part 1 extra credit — Lovable Mom and configurable student processes: **25/25**
- Part 2 — Thread synchronization using monitors and condition variables: **100/100**

## Repository structure

- `part-1-semaphores/` — Shared bank account protected by a process-shared POSIX semaphore.
- `part-2-monitors/` — Ben's Chili Bowl producer-consumer simulation using pthread mutexes and condition variables.

## Build and run

### Part 1

```bash
cd part-1-semaphores
make
./shm_proc
```

Extra-credit command-line form:

```bash
./shm_proc <parents: 1|2> <students>
```

Example:

```bash
./shm_proc 2 10
```

The simulation runs indefinitely. Stop it with `Ctrl+C` or a process signal.

### Part 2

```bash
cd part-2-monitors
make
./main
```

## Synchronization concepts

Part 1 uses shared memory created with `mmap` and a POSIX semaphore initialized with `pshared=1`. Every bank-account read/modify/write is protected by the semaphore.

Part 2 implements a bounded FIFO order queue as a monitor. A pthread mutex protects restaurant state, while `can_add_orders` and `can_get_orders` condition variables coordinate customer and cook threads.

## Attribution

Author: Selorm Kalitsi

Development assistance: OpenAI ChatGPT/Codex was used for implementation support, testing, and documentation. The final programs were compiled, exercised with concurrent runtime tests, and evaluated with the official Codio graders.
