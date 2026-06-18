/**
 * @file simulator_message.c
 * @brief Message queue and publish/receive logic for the pub/sub simulator.
 */

#include "simulator_internal.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a message node for insertion into a topic queue.
 *
 * @param message Message payload string.
 * @param priority Message priority value.
 * @return A newly allocated Message, or NULL if allocation fails.
 */
static Message *create_message_node(const char *message, int priority);

/**
 * @brief Inserts a message into a topic queue in priority order.
 *
 * Higher-priority messages are placed closer to the front of the queue.
 * Messages with the same priority remain in first-in, first-out order.
 *
 * @param topic Topic whose queue will receive the message.
 * @param new_message Message node to insert.
 */
static void enqueue_message_by_priority(Topic *topic, Message *new_message);

/**
 * @brief Validates a message payload string.
 *
 * @param message Message string to validate.
 * @return 1 if the payload is valid, or 0 otherwise.
 */
static int is_valid_message(const char *message)
{
    return message != NULL && message[0] != '\0' && strlen(message) < SIM_NAME_LENGTH;
}

static Message *create_message_node(const char *message, int priority)
{
    Message *new_message = (Message *)malloc(sizeof(Message));

    if (new_message == NULL) {
        return NULL;
    }

    strcpy(new_message->data, message);
    new_message->priority = priority;
    new_message->next = NULL;
    return new_message;
}

static void enqueue_message_by_priority(Topic *topic, Message *new_message)
{
    Message *current;

    if (topic == NULL || new_message == NULL) {
        return;
    }

    if (topic->message_head == NULL) {
        topic->message_head = new_message;
        topic->message_tail = new_message;
        return;
    }

    if (new_message->priority > topic->message_head->priority) {
        new_message->next = topic->message_head;
        topic->message_head = new_message;
        return;
    }

    current = topic->message_head;
    while (current->next != NULL && current->next->priority >= new_message->priority) {
        current = current->next;
    }

    new_message->next = current->next;
    current->next = new_message;

    if (new_message->next == NULL) {
        topic->message_tail = new_message;
    }
}

/**
 * @brief Removes and returns the front message from a topic queue.
 *
 * @param topic Topic whose queue should be dequeued.
 * @return The removed Message, or NULL if the queue is empty.
 */
Message *simulator_dequeue_message(Topic *topic)
{
    Message *message;

    if (topic == NULL || topic->message_head == NULL) {
        return NULL;
    }

    message = topic->message_head;
    topic->message_head = message->next;
    if (topic->message_head == NULL) {
        topic->message_tail = NULL;
    }
    message->next = NULL;
    return message;
}

/**
 * @brief Receives the next queued message for a subscriber.
 *
 * @param sim Simulator instance.
 * @param node_name Subscriber node name.
 * @param topic_name Topic name.
 * @return A heap-allocated Message on success, or NULL on failure.
 */
Message *simulator_receive_message(Simulator *sim, const char *node_name, const char *topic_name)
{
    Topic *topic;

    if (sim == NULL || !is_valid_name(node_name) || !is_valid_name(topic_name)) {
        return NULL;
    }

    if (simulator_find_node(sim, node_name) == NULL) {
        return NULL;
    }

    topic = simulator_find_topic(sim, topic_name);
    if (topic == NULL) {
        return NULL;
    }

    if (find_subscriber_in_topic(topic, node_name) == NULL) {
        return NULL;
    }

    return simulator_dequeue_message(topic);
}

/**
 * @brief Publishes a message to a topic queue after validating the request.
 *
 * The function checks that the node exists, the topic exists, the node is
 * registered as a publisher for that topic, and the message payload is valid.
 * On success, the message is inserted into the topic queue.
 *
 * @param sim Simulator instance.
 * @param node_name Publisher node name.
 * @param topic_name Target topic name.
 * @param message Message payload.
 * @param priority Message priority value.
 * @return 1 on success, or 0 if validation or allocation fails.
 */
int simulator_publish_message(Simulator *sim, const char *node_name, const char *topic_name, const char *message, int priority)
{
    Topic *topic;
    Message *new_message;

    if (sim == NULL ||
        !is_valid_name(node_name) ||
        !is_valid_name(topic_name) ||
        !is_valid_message(message)) {
        return 0;
    }

    if (simulator_find_node(sim, node_name) == NULL) {
        return 0;
    }

    topic = simulator_find_topic(sim, topic_name);
    if (topic == NULL) {
        return 0;
    }

    if (find_publisher_in_topic(topic, node_name) == NULL) {
        return 0;
    }

    new_message = create_message_node(message, priority);
    if (new_message == NULL) {
        return 0;
    }

    enqueue_message_by_priority(topic, new_message);
    return 1;
}
