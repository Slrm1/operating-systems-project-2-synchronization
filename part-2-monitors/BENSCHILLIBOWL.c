/*
 * Project 2 Part 2 - Restaurant monitor implementation
 * Author: Selorm Kalitsi
 * Development assistance: OpenAI ChatGPT/Codex.
 */
#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

bool IsEmpty(BENSCHILLIBOWL *bcb);
bool IsFull(BENSCHILLIBOWL *bcb);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = {
    "BensChilli",
    "BensHalfSmoke",
    "BensHotDog",
    "BensChilliCheeseFries",
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings"
};
int BENSCHILLIBOWLMenuLength = 10;

MenuItem PickRandomMenuItem(void) {
    return BENSCHILLIBOWLMenu[rand() % BENSCHILLIBOWLMenuLength];
}

BENSCHILLIBOWL *OpenRestaurant(int max_size, int expected_num_orders) {
    if (max_size <= 0 || expected_num_orders < 0) return NULL;

    BENSCHILLIBOWL *bcb = calloc(1, sizeof(*bcb));
    if (bcb == NULL) return NULL;

    bcb->orders = NULL;
    bcb->current_size = 0;
    bcb->max_size = max_size;
    bcb->next_order_number = 1;
    bcb->orders_handled = 0;
    bcb->expected_num_orders = expected_num_orders;

    if (pthread_mutex_init(&bcb->mutex, NULL) != 0) {
        free(bcb);
        return NULL;
    }
    if (pthread_cond_init(&bcb->can_add_orders, NULL) != 0) {
        pthread_mutex_destroy(&bcb->mutex);
        free(bcb);
        return NULL;
    }
    if (pthread_cond_init(&bcb->can_get_orders, NULL) != 0) {
        pthread_cond_destroy(&bcb->can_add_orders);
        pthread_mutex_destroy(&bcb->mutex);
        free(bcb);
        return NULL;
    }

    printf("Restaurant is open!\n");
    return bcb;
}

void CloseRestaurant(BENSCHILLIBOWL *bcb) {
    if (bcb == NULL) return;

    assert(bcb->orders_handled == bcb->expected_num_orders);
    assert(bcb->current_size == 0);
    assert(bcb->orders == NULL);

    pthread_cond_destroy(&bcb->can_add_orders);
    pthread_cond_destroy(&bcb->can_get_orders);
    pthread_mutex_destroy(&bcb->mutex);
    free(bcb);
    printf("Restaurant is closed!\n");
}

int AddOrder(BENSCHILLIBOWL *bcb, Order *order) {
    if (bcb == NULL || order == NULL) return -1;

    pthread_mutex_lock(&bcb->mutex);
    while (IsFull(bcb)) {
        pthread_cond_wait(&bcb->can_add_orders, &bcb->mutex);
    }

    order->order_number = bcb->next_order_number++;
    order->next = NULL;
    AddOrderToBack(&bcb->orders, order);
    bcb->current_size++;
    int order_number = order->order_number;

    pthread_cond_signal(&bcb->can_get_orders);
    pthread_mutex_unlock(&bcb->mutex);
    return order_number;
}

Order *GetOrder(BENSCHILLIBOWL *bcb) {
    if (bcb == NULL) return NULL;

    pthread_mutex_lock(&bcb->mutex);
    while (IsEmpty(bcb) &&
           bcb->orders_handled < bcb->expected_num_orders) {
        pthread_cond_wait(&bcb->can_get_orders, &bcb->mutex);
    }

    if (IsEmpty(bcb) &&
        bcb->orders_handled >= bcb->expected_num_orders) {
        printf("No more orders\n");
        pthread_cond_broadcast(&bcb->can_get_orders);
        pthread_mutex_unlock(&bcb->mutex);
        return NULL;
    }

    Order *order = bcb->orders;
    bcb->orders = order->next;
    order->next = NULL;
    bcb->current_size--;
    bcb->orders_handled++;

    pthread_cond_signal(&bcb->can_add_orders);
    if (bcb->orders_handled == bcb->expected_num_orders) {
        pthread_cond_broadcast(&bcb->can_get_orders);
    }

    pthread_mutex_unlock(&bcb->mutex);
    return order;
}

bool IsEmpty(BENSCHILLIBOWL *bcb) {
    return bcb == NULL || bcb->current_size == 0;
}

bool IsFull(BENSCHILLIBOWL *bcb) {
    return bcb != NULL && bcb->current_size >= bcb->max_size;
}

void AddOrderToBack(Order **orders, Order *order) {
    if (orders == NULL || order == NULL) return;

    order->next = NULL;
    if (*orders == NULL) {
        *orders = order;
        return;
    }

    Order *tail = *orders;
    while (tail->next != NULL) tail = tail->next;
    tail->next = order;
}
