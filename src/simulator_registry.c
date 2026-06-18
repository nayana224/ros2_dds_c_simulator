/**
 * @file simulator_registry.c
 * @brief Node, topic, publisher, and subscriber registration logic.
 */

#include "simulator_internal.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Validates a node or topic name.
 *
 * @param name Name string to validate.
 * @return 1 if the name is valid, or 0 otherwise.
 */
int is_valid_name(const char *name)
{
    return name != NULL && name[0] != '\0' && strlen(name) < SIM_NAME_LENGTH;
}

/**
 * @brief Finds a registered node by name.
 *
 * @param sim Simulator instance.
 * @param name Node name to search for.
 * @return A pointer to the matching node, or NULL if not found.
 */
Node *simulator_find_node(const Simulator *sim, const char *name)
{
    Node *current;

    if (sim == NULL || name == NULL) {
        return NULL;
    }

    current = sim->nodes;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/**
 * @brief Finds a registered topic by name.
 *
 * @param sim Simulator instance.
 * @param name Topic name to search for.
 * @return A pointer to the matching topic, or NULL if not found.
 */
Topic *simulator_find_topic(const Simulator *sim, const char *name)
{
    Topic *current;

    if (sim == NULL || name == NULL) {
        return NULL;
    }

    current = sim->topics;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/**
 * @brief Exposes publisher lookup as part of the public API.
 *
 * @param topic Topic to search.
 * @param node_name Publisher node name to search for.
 * @return A pointer to the matching publisher entry, or NULL if not found.
 */
Publisher *simulator_find_publisher(const Topic *topic, const char *node_name)
{
    return find_publisher_in_topic(topic, node_name);
}

/**
 * @brief Exposes subscriber lookup as part of the public API.
 *
 * @param topic Topic to search.
 * @param node_name Subscriber node name to search for.
 * @return A pointer to the matching subscriber entry, or NULL if not found.
 */
Subscriber *simulator_find_subscriber(const Topic *topic, const char *node_name)
{
    return find_subscriber_in_topic(topic, node_name);
}

Publisher *find_publisher_in_topic(const Topic *topic, const char *node_name)
{
    Publisher *current;

    if (topic == NULL || node_name == NULL) {
        return NULL;
    }

    current = topic->publishers;
    while (current != NULL) {
        if (strcmp(current->node_name, node_name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

Subscriber *find_subscriber_in_topic(const Topic *topic, const char *node_name)
{
    Subscriber *current;

    if (topic == NULL || node_name == NULL) {
        return NULL;
    }

    current = topic->subscribers;
    while (current != NULL) {
        if (strcmp(current->node_name, node_name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/**
 * @brief Registers a new node by prepending it to the node list.
 *
 * @param sim Simulator instance to modify.
 * @param name Node name to register.
 * @return 1 on success, or 0 if validation or allocation fails.
 */
int simulator_add_node(Simulator *sim, const char *name)
{
    Node *new_node;

    if (sim == NULL || !is_valid_name(name)) {
        return 0;
    }

    if (simulator_find_node(sim, name) != NULL) {
        return 0;
    }

    new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        return 0;
    }

    strcpy(new_node->name, name);
    new_node->next = sim->nodes;
    sim->nodes = new_node;

    return 1;
}

/**
 * @brief Registers a new topic by prepending it to the topic list.
 *
 * @param sim Simulator instance to modify.
 * @param name Topic name to register.
 * @return 1 on success, or 0 if validation or allocation fails.
 */
int simulator_add_topic(Simulator *sim, const char *name)
{
    Topic *new_topic;

    if (sim == NULL || !is_valid_name(name)) {
        return 0;
    }

    if (simulator_find_topic(sim, name) != NULL) {
        return 0;
    }

    new_topic = (Topic *)malloc(sizeof(Topic));
    if (new_topic == NULL) {
        return 0;
    }

    strcpy(new_topic->name, name);
    new_topic->publishers = NULL;
    new_topic->subscribers = NULL;
    new_topic->message_head = NULL;
    new_topic->message_tail = NULL;
    new_topic->next = sim->topics;
    sim->topics = new_topic;

    return 1;
}

/**
 * @brief Registers a node as a publisher for a topic.
 *
 * @param sim Simulator instance to modify.
 * @param node_name Publisher node name.
 * @param topic_name Topic name.
 * @return 1 on success, or 0 if validation or allocation fails.
 */
int simulator_add_publisher(Simulator *sim, const char *node_name, const char *topic_name)
{
    Topic *topic;
    Publisher *new_publisher;

    if (sim == NULL || !is_valid_name(node_name) || !is_valid_name(topic_name)) {
        return 0;
    }

    if (simulator_find_node(sim, node_name) == NULL) {
        return 0;
    }

    topic = simulator_find_topic(sim, topic_name);
    if (topic == NULL) {
        return 0;
    }

    if (find_publisher_in_topic(topic, node_name) != NULL) {
        return 0;
    }

    new_publisher = (Publisher *)malloc(sizeof(Publisher));
    if (new_publisher == NULL) {
        return 0;
    }

    strcpy(new_publisher->node_name, node_name);
    new_publisher->next = topic->publishers;
    topic->publishers = new_publisher;
    return 1;
}

/**
 * @brief Registers a node as a subscriber for a topic.
 *
 * @param sim Simulator instance to modify.
 * @param node_name Subscriber node name.
 * @param topic_name Topic name.
 * @return 1 on success, or 0 if validation or allocation fails.
 */
int simulator_add_subscriber(Simulator *sim, const char *node_name, const char *topic_name)
{
    Topic *topic;
    Subscriber *new_subscriber;

    if (sim == NULL || !is_valid_name(node_name) || !is_valid_name(topic_name)) {
        return 0;
    }

    if (simulator_find_node(sim, node_name) == NULL) {
        return 0;
    }

    topic = simulator_find_topic(sim, topic_name);
    if (topic == NULL) {
        return 0;
    }

    if (find_subscriber_in_topic(topic, node_name) != NULL) {
        return 0;
    }

    new_subscriber = (Subscriber *)malloc(sizeof(Subscriber));
    if (new_subscriber == NULL) {
        return 0;
    }

    strcpy(new_subscriber->node_name, node_name);
    new_subscriber->next = topic->subscribers;
    topic->subscribers = new_subscriber;
    return 1;
}
