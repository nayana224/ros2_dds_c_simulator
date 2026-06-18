/**
 * @file simulator_lifecycle.c
 * @brief Simulator lifecycle and cleanup implementation.
 */

#include "simulator_internal.h"

#include <stdlib.h>

/**
 * @brief Releases every node in a linked node list.
 *
 * @param head Head of the node list to destroy.
 */
static void destroy_node_list(Node *head)
{
    Node *current = head;

    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
}

/**
 * @brief Releases an entire topic list and all nested allocations.
 *
 * Each topic owns its publisher list, subscriber list, and message queue.
 * This function walks the full structure and frees every owned object.
 *
 * @param head Head of the topic list to destroy.
 */
static void destroy_topic_list(Topic *head)
{
    Topic *current = head;

    while (current != NULL) {
        Topic *next = current->next;
        Message *message = current->message_head;
        Publisher *publisher = current->publishers;
        Subscriber *subscriber = current->subscribers;

        while (message != NULL) {
            Message *message_next = message->next;
            free(message);
            message = message_next;
        }

        while (publisher != NULL) {
            Publisher *publisher_next = publisher->next;
            free(publisher);
            publisher = publisher_next;
        }

        while (subscriber != NULL) {
            Subscriber *subscriber_next = subscriber->next;
            free(subscriber);
            subscriber = subscriber_next;
        }

        free(current);
        current = next;
    }
}

/**
 * @brief Initializes a Simulator to the empty state.
 *
 * @param sim Simulator instance to initialize.
 */
void simulator_init(Simulator *sim)
{
    if (sim == NULL) {
        return;
    }

    sim->nodes = NULL;
    sim->topics = NULL;
}

/**
 * @brief Releases all dynamically allocated state owned by the simulator.
 *
 * @param sim Simulator instance to destroy.
 */
void simulator_destroy(Simulator *sim)
{
    if (sim == NULL) {
        return;
    }

    destroy_node_list(sim->nodes);
    destroy_topic_list(sim->topics);
    sim->nodes = NULL;
    sim->topics = NULL;
}
