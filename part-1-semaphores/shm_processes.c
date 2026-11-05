/*
 * Project 2 Part 1 - Process Synchronization Using Semaphores
 * Author: Selorm Kalitsi
 *
 * Development assistance: OpenAI ChatGPT/Codex.
 *
 * Dear Old Dad and Poor Student share a bank account. A process-shared
 * semaphore protects every read/modify/write of the account. Optional
 * command-line arguments enable the extra-credit Mom and N students:
 *     ./shm_proc [number_of_parents: 1|2] [number_of_students]
 */
#define _DEFAULT_SOURCE
#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int BankAccount;
    sem_t mutex;
} SharedData;

static SharedData *shared = NULL;

static void lock_account(void) {
    while (sem_wait(&shared->mutex) == -1 && errno == EINTR) { }
}

static void unlock_account(void) {
    sem_post(&shared->mutex);
}

static void pause_random(unsigned int *seed, int maximum) {
    sleep((unsigned int)(rand_r(seed) % (maximum + 1)));
}

static void dear_old_dad(void) {
    unsigned int seed = (unsigned int)(time(NULL) ^ getpid());
    int localBalance = 0;

    for (;;) {
        pause_random(&seed, 5);
        printf("Dear Old Dad: Attempting to Check Balance\n");

        lock_account();
        localBalance = shared->BankAccount;
        if ((rand_r(&seed) % 2) == 0) {
            if (localBalance < 100) {
                int amount = (int)(rand_r(&seed) % 101);
                if ((rand_r(&seed) % 2) == 0) {
                    localBalance += amount;
                    shared->BankAccount = localBalance;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n",
                           amount, localBalance);
                } else {
                    printf("Dear old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n",
                       localBalance);
            }
        } else {
            printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
        }
        unlock_account();
    }
}

static void poor_student(int student_number) {
    unsigned int seed =
        (unsigned int)(time(NULL) ^ getpid() ^ (student_number << 8));
    int localBalance = 0;

    for (;;) {
        pause_random(&seed, 5);
        printf("Poor Student: Attempting to Check Balance\n");

        lock_account();
        localBalance = shared->BankAccount;
        if ((rand_r(&seed) % 2) == 0) {
            int need = (int)(rand_r(&seed) % 51);
            printf("Poor Student needs $%d\n", need);
            if (need <= localBalance) {
                localBalance -= need;
                shared->BankAccount = localBalance;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n",
                       need, localBalance);
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
            }
        } else {
            printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
        }
        unlock_account();
    }
}

static void lovable_mom(void) {
    unsigned int seed = (unsigned int)(time(NULL) ^ getpid());
    int localBalance = 0;

    for (;;) {
        pause_random(&seed, 10);
        printf("Loveable Mom: Attempting to Check Balance\n");

        lock_account();
        localBalance = shared->BankAccount;
        if (localBalance <= 100) {
            int amount = (int)(rand_r(&seed) % 126);
            localBalance += amount;
            shared->BankAccount = localBalance;
            printf("Lovable Mom: Deposits $%d / Balance = $%d\n",
                   amount, localBalance);
        }
        unlock_account();
    }
}

int main(int argc, char *argv[]) {
    int parent_count = 1;
    int student_count = 1;

    if (argc > 1) parent_count = atoi(argv[1]);
    if (argc > 2) student_count = atoi(argv[2]);
    if (parent_count < 1 || parent_count > 2 || student_count < 1) {
        fprintf(stderr,
                "Usage: %s [parents: 1|2] [students: positive integer]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    setvbuf(stdout, NULL, _IONBF, 0);
    shared = mmap(NULL, sizeof(*shared), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    shared->BankAccount = 0;
    if (sem_init(&shared->mutex, 1, 1) == -1) {
        perror("sem_init");
        munmap(shared, sizeof(*shared));
        return EXIT_FAILURE;
    }

    if (parent_count == 2) {
        pid_t mom = fork();
        if (mom == -1) {
            perror("fork");
            return EXIT_FAILURE;
        }
        if (mom == 0) lovable_mom();
    }

    for (int i = 1; i <= student_count; ++i) {
        pid_t child = fork();
        if (child == -1) {
            perror("fork");
            return EXIT_FAILURE;
        }
        if (child == 0) poor_student(i);
    }

    dear_old_dad();

    while (wait(NULL) > 0) { }
    sem_destroy(&shared->mutex);
    munmap(shared, sizeof(*shared));
    return EXIT_SUCCESS;
}
