/**
 * @file simulator.h
 * @brief Public API for a lightweight ROS2-style pub/sub simulator.
 *
 * This header defines the data structures and functions used to model nodes,
 * topics, publishers, subscribers, queued messages, and communication paths
 * in a single-process C program.
 *
 * The simulator does not use ROS2, DDS, RTPS, sockets, or external networking
 * libraries. Instead, it represents communication with linked lists, a
 * per-topic priority queue, and graph-style path traversal.
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stddef.h>

/**
 * @brief Fixed-size buffer length used for node, topic, and message strings.
 */
#define SIM_NAME_LENGTH 64

typedef struct Message Message;

/**
 * @brief A registered node in the simulator.
 *
 * Nodes are stored in a singly linked list owned by the Simulator.
 */
typedef struct Node {
    char name[SIM_NAME_LENGTH]; /**< Node name. */
    struct Node *next; /**< Next node in the registered node list. */
} Node;

/**
 * @brief A topic registered in the simulator.
 *
 * Each topic owns a publisher list, a subscriber list, and a message queue
 * ordered by priority.
 */
typedef struct Topic {
    char name[SIM_NAME_LENGTH]; /**< Topic name. */
    struct Publisher *publishers; /**< Head of the publisher list for this topic. */
    struct Subscriber *subscribers; /**< Head of the subscriber list for this topic. */
    Message *message_head; /**< Front of the per-topic message queue. */
    Message *message_tail; /**< Rear of the per-topic message queue. */
    struct Topic *next; /**< Next topic in the registered topic list. */
} Topic;

/**
 * @brief A publisher entry attached to a topic.
 *
 * The entry stores the publisher node name rather than a direct pointer to a
 * Node object.
 */
typedef struct Publisher {
    char node_name[SIM_NAME_LENGTH]; /**< Name of the registered publisher node. */
    struct Publisher *next; /**< Next publisher in the topic publisher list. */
} Publisher;

/**
 * @brief A subscriber entry attached to a topic.
 *
 * The entry stores the subscriber node name rather than a direct pointer to a
 * Node object.
 */
typedef struct Subscriber {
    char node_name[SIM_NAME_LENGTH]; /**< Name of the registered subscriber node. */
    struct Subscriber *next; /**< Next subscriber in the topic subscriber list. */
} Subscriber;

/**
 * @brief A message stored in a topic queue.
 *
 * Messages are linked together in descending priority order. Messages with the
 * same priority preserve publish order.
 */
typedef struct Message {
    char data[SIM_NAME_LENGTH]; /**< Message payload string. */
    int priority; /**< Message priority value. Higher values are delivered first. */
    struct Message *next; /**< Next message in the topic queue. */
} Message;

/**
 * @brief Global simulator state.
 *
 * A Simulator owns the registered node list and topic list. Each topic in
 * turn owns its publisher list, subscriber list, and queued messages.
 */
typedef struct Simulator {
    Node *nodes; /**< Head of the registered node list. */
    Topic *topics; /**< Head of the registered topic list. */
} Simulator;

/**
 * @brief Initializes a Simulator to the empty state.
 *
 * @param sim Simulator instance to initialize.
 */
void simulator_init(Simulator *sim);

/**
 * @brief Releases all dynamically allocated state owned by the simulator.
 *
 * This function frees every registered node, topic, publisher entry,
 * subscriber entry, and queued message, then resets the simulator to the
 * empty state.
 *
 * @param sim Simulator instance to destroy.
 */
void simulator_destroy(Simulator *sim);

/**
 * @brief Registers a new node in the simulator.
 *
 * The node name must be non-NULL, non-empty, shorter than SIM_NAME_LENGTH,
 * and unique within the simulator.
 *
 * @param sim Simulator instance to modify.
 * @param name Node name to register.
 * @return 1 if the node was registered successfully, or 0 on failure.
 */
int simulator_add_node(Simulator *sim, const char *name);

/**
 * @brief Registers a new topic in the simulator.
 *
 * The topic name must be non-NULL, non-empty, shorter than SIM_NAME_LENGTH,
 * and unique within the simulator.
 *
 * @param sim Simulator instance to modify.
 * @param name Topic name to register.
 * @return 1 if the topic was registered successfully, or 0 on failure.
 */
int simulator_add_topic(Simulator *sim, const char *name);

/**
 * @brief Finds a registered node by name.
 *
 * @param sim Simulator instance.
 * @param name Node name to search for.
 * @return A pointer to the matching node, or NULL if not found.
 */
Node *simulator_find_node(const Simulator *sim, const char *name);

/**
 * @brief Finds a registered topic by name.
 *
 * @param sim Simulator instance.
 * @param name Topic name to search for.
 * @return A pointer to the matching topic, or NULL if not found.
 */
Topic *simulator_find_topic(const Simulator *sim, const char *name);

/**
 * @brief Formats the registered node list into a text buffer.
 *
 * @param sim Simulator instance.
 * @param node_buffer Output buffer for the formatted node list.
 * @param node_buffer_size Size of @p node_buffer in bytes.
 * @return 1 if formatting succeeds, or 0 if the node list is empty or invalid.
 */
int simulator_format_nodes(
    const Simulator *sim,
    char *node_buffer,
    size_t node_buffer_size
);

/**
 * @brief Formats the registered topic list into a text buffer.
 *
 * The formatted output includes topic names and their publisher/subscriber
 * relationships.
 *
 * @param sim Simulator instance.
 * @param topic_buffer Output buffer for the formatted topic list.
 * @param topic_buffer_size Size of @p topic_buffer in bytes.
 * @return 1 if formatting succeeds, or 0 if the topic list is empty or invalid.
 */
int simulator_format_topics(
    const Simulator *sim,
    char *topic_buffer,
    size_t topic_buffer_size
);

/**
 * @brief Formats the registered node list and topic list into a text buffer.
 *
 * @param sim Simulator instance.
 * @param list_buffer Output buffer for the formatted list text.
 * @param list_buffer_size Size of @p list_buffer in bytes.
 * @return 1 if formatting succeeds, or 0 if both lists are empty or invalid.
 */
int simulator_format_registered_lists(
    const Simulator *sim,
    char *list_buffer,
    size_t list_buffer_size
);

/**
 * @brief Formats the communication graph into a text buffer.
 *
 * The graph is formatted as directed edges from publisher nodes to topics and
 * from topics to subscriber nodes, separated by newlines.
 *
 * @param sim Simulator instance.
 * @param graph_buffer Output buffer for the formatted graph text.
 * @param graph_buffer_size Size of @p graph_buffer in bytes.
 * @return 1 if formatting succeeds, or 0 if the graph cannot be written.
 */
int simulator_format_communication_graph(
    const Simulator *sim,
    char *graph_buffer,
    size_t graph_buffer_size
);

/**
 * @brief Builds a directed communication path string between nodes.
 *
 * The search treats publisher-to-topic and topic-to-subscriber relationships
 * as directed edges and uses breadth-first search (BFS) to find a reachable
 * path from the start node to the target node.
 *
 * On success, the path is written to @p path_buffer in the form:
 * `node -> topic -> node -> ...`
 *
 * @param sim Simulator instance.
 * @param start_node_name Name of the source node.
 * @param target_node_name Name of the destination node.
 * @param path_buffer Output buffer for the formatted path string.
 * @param path_buffer_size Size of @p path_buffer in bytes.
 * @return 1 if a path is found and written successfully, or 0 otherwise.
 */
int simulator_format_path_between_nodes(
    const Simulator *sim,
    const char *start_node_name,
    const char *target_node_name,
    char *path_buffer,
    size_t path_buffer_size
);

/**
 * @brief Registers a node as a publisher for a topic.
 *
 * Both the node and topic must already exist. The same node cannot be
 * registered as a publisher for the same topic more than once.
 *
 * @param sim Simulator instance to modify.
 * @param node_name Node name to register as a publisher.
 * @param topic_name Topic name to attach the publisher to.
 * @return 1 if the publisher was registered successfully, or 0 on failure.
 */
int simulator_add_publisher(Simulator *sim, const char *node_name, const char *topic_name);

/**
 * @brief Registers a node as a subscriber for a topic.
 *
 * Both the node and topic must already exist. The same node cannot be
 * registered as a subscriber for the same topic more than once.
 *
 * @param sim Simulator instance to modify.
 * @param node_name Node name to register as a subscriber.
 * @param topic_name Topic name to attach the subscriber to.
 * @return 1 if the subscriber was registered successfully, or 0 on failure.
 */
int simulator_add_subscriber(Simulator *sim, const char *node_name, const char *topic_name);

/**
 * @brief Publishes a message to a topic.
 *
 * The given node must already be registered as a publisher of the target
 * topic. On success, the message is inserted into the topic queue according
 * to priority order. Higher-priority messages are received first. Messages
 * with the same priority preserve publish order.
 *
 * @param sim Simulator instance.
 * @param node_name Publisher node name.
 * @param topic_name Target topic name.
 * @param message Message payload.
 * @param priority Message priority value.
 * @return 1 on success, or 0 if validation or insertion fails.
 */
int simulator_publish_message(Simulator *sim, const char *node_name, const char *topic_name, const char *message, int priority);

/**
 * @brief Receives the next available message from a subscribed topic.
 *
 * The given node must already be registered as a subscriber of the target
 * topic. On success, this function removes and returns the front message from
 * the topic queue.
 *
 * @param sim Simulator instance.
 * @param node_name Subscriber node name.
 * @param topic_name Target topic name.
 * @return A heap-allocated Message on success, or NULL if the request is
 *         invalid or the queue is empty.
 *
 * @warning The caller owns the returned Message and must free it.
 */
Message *simulator_receive_message(Simulator *sim, const char *node_name, const char *topic_name);

/**
 * @brief Removes and returns the front message from a topic queue.
 *
 * @param topic Topic whose queue should be dequeued.
 * @return The removed Message, or NULL if the queue is empty.
 *
 * @warning The caller owns the returned Message and must free it.
 */
Message *simulator_dequeue_message(Topic *topic);

/**
 * @brief Finds a publisher entry by node name within a topic.
 *
 * @param topic Topic to search.
 * @param node_name Publisher node name to search for.
 * @return A pointer to the matching publisher entry, or NULL if not found.
 */
Publisher *simulator_find_publisher(const Topic *topic, const char *node_name);

/**
 * @brief Finds a subscriber entry by node name within a topic.
 *
 * @param topic Topic to search.
 * @param node_name Subscriber node name to search for.
 * @return A pointer to the matching subscriber entry, or NULL if not found.
 */
Subscriber *simulator_find_subscriber(const Topic *topic, const char *node_name);

#endif
