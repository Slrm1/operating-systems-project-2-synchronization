#ifndef BENSCHILLIBOWL_H
#define BENSCHILLIBOWL_H

#include <stdbool.h>
#include <pthread.h>

typedef char *MenuItem;

typedef struct OrderStruct {
    MenuItem menu_item;
    int customer_id;
    int order_number;
    struct OrderStruct *next;
} Order;

typedef struct Restaurant {
    Order *orders;
    int current_size;
    int max_size;
    int next_order_number;
    int orders_handled;
    int expected_num_orders;
    pthread_mutex_t mutex;
    pthread_cond_t can_add_orders;
    pthread_cond_t can_get_orders;
} BENSCHILLIBOWL;

MenuItem PickRandomMenuItem(void);
BENSCHILLIBOWL *OpenRestaurant(int max_size, int expected_num_orders);
void CloseRestaurant(BENSCHILLIBOWL *bcb);
int AddOrder(BENSCHILLIBOWL *bcb, Order *order);
Order *GetOrder(BENSCHILLIBOWL *bcb);

#endif
