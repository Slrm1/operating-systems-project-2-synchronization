/*
 * Project 2 Part 2 - Thread synchronization driver
 * Author: Selorm Kalitsi
 * Development assistance: OpenAI ChatGPT/Codex.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "BENSCHILLIBOWL.h"

#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS (NUM_CUSTOMERS * ORDERS_PER_CUSTOMER)

BENSCHILLIBOWL *bcb;

void *BENSCHILLIBOWLCustomer(void *tid) {
    int customer_id = (int)(long)tid;

    for (int i = 0; i < ORDERS_PER_CUSTOMER; ++i) {
        Order *order = malloc(sizeof(*order));
        if (order == NULL) {
            perror("malloc");
            return (void *)1;
        }

        order->menu_item = PickRandomMenuItem();
        order->customer_id = customer_id;
        order->order_number = 0;
        order->next = NULL;

        if (AddOrder(bcb, order) < 0) {
            free(order);
            return (void *)1;
        }
    }

    return NULL;
}

void *BENSCHILLIBOWLCook(void *tid) {
    int cook_id = (int)(long)tid;
    int orders_fulfilled = 0;
    Order *order;

    while ((order = GetOrder(bcb)) != NULL) {
        orders_fulfilled++;
        free(order);
    }

    printf("Cook #%d fulfilled %d orders\n",
           cook_id, orders_fulfilled);
    return NULL;
}

int main(void) {
    pthread_t customers[NUM_CUSTOMERS];
    pthread_t cooks[NUM_COOKS];
    int customers_created = 0;
    int cooks_created = 0;

    srand((unsigned int)time(NULL));
    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE,
                         EXPECTED_NUM_ORDERS);
    if (bcb == NULL) {
        fprintf(stderr, "Unable to open restaurant\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_COOKS; ++i) {
        if (pthread_create(&cooks[i], NULL,
                           BENSCHILLIBOWLCook,
                           (void *)(long)i) != 0) {
            fprintf(stderr,
                    "Unable to create cook thread %d\n", i);
            return EXIT_FAILURE;
        }
        cooks_created++;
    }

    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        if (pthread_create(&customers[i], NULL,
                           BENSCHILLIBOWLCustomer,
                           (void *)(long)i) != 0) {
            fprintf(stderr,
                    "Unable to create customer thread %d\n", i);
            return EXIT_FAILURE;
        }
        customers_created++;
    }

    for (int i = 0; i < customers_created; ++i) {
        pthread_join(customers[i], NULL);
    }
    for (int i = 0; i < cooks_created; ++i) {
        pthread_join(cooks[i], NULL);
    }

    CloseRestaurant(bcb);
    bcb = NULL;
    return EXIT_SUCCESS;
}
