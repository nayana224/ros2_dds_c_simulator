/**
 * @file simulator_graph.c
 * @brief Graph traversal and output formatting logic for the pub/sub simulator.
 */

#include "simulator_internal.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Counts how many nodes are currently registered.
 *
 * @param sim Simulator instance.
 * @return Number of registered nodes.
 */
static int count_nodes(const Simulator *sim);

/**
 * @brief Copies registered node pointers into an array.
 *
 * @param sim Simulator instance.
 * @param nodes Output array that receives node pointers.
 */
static void collect_nodes(const Simulator *sim, const Node **nodes);

/**
 * @brief Finds the array index of a node name in a collected node list.
 *
 * @param nodes Array of node pointers.
 * @param node_count Number of entries in @p nodes.
 * @param name Node name to search for.
 * @return Matching index, or -1 if not found.
 */
static int find_node_index(const Node **nodes, int node_count, const char *name);

/**
 * @brief Appends text to a bounded output buffer.
 *
 * @param buffer Destination buffer.
 * @param buffer_size Size of @p buffer in bytes.
 * @param offset Current write offset.
 * @param text Text to append.
 * @return 1 if the text was appended successfully, or 0 on overflow.
 */
static int append_path_text(char *buffer, size_t buffer_size, size_t *offset, const char *text);

/**
 * @brief Appends a decimal index and a trailing period to an output buffer.
 *
 * @param buffer Destination buffer.
 * @param buffer_size Size of @p buffer in bytes.
 * @param offset Current write offset.
 * @param value Integer value to append.
 * @return 1 if the value was appended successfully, or 0 on overflow.
 */
static int append_index_text(char *buffer, size_t buffer_size, size_t *offset, int value);

static int count_nodes(const Simulator *sim)
{
    const Node *current;
    int count = 0;

    if (sim == NULL) {
        return 0;
    }

    current = sim->nodes;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

static void collect_nodes(const Simulator *sim, const Node **nodes)
{
    const Node *current;
    int index = 0;

    if (sim == NULL || nodes == NULL) {
        return;
    }

    current = sim->nodes;
    while (current != NULL) {
        nodes[index] = current;
        index++;
        current = current->next;
    }
}

static int find_node_index(const Node **nodes, int node_count, const char *name)
{
    int index;

    if (nodes == NULL || name == NULL) {
        return -1;
    }

    for (index = 0; index < node_count; index++) {
        if (strcmp(nodes[index]->name, name) == 0) {
            return index;
        }
    }

    return -1;
}

static int append_path_text(char *buffer, size_t buffer_size, size_t *offset, const char *text)
{
    size_t text_length;

    if (buffer == NULL || offset == NULL || text == NULL || *offset >= buffer_size) {
        return 0;
    }

    text_length = strlen(text);
    if (*offset + text_length + 1 > buffer_size) {
        return 0;
    }

    memcpy(buffer + *offset, text, text_length);
    *offset += text_length;
    buffer[*offset] = '\0';
    return 1;
}

static int append_index_text(char *buffer, size_t buffer_size, size_t *offset, int value)
{
    char index_buffer[32];
    int written;

    written = snprintf(index_buffer, sizeof(index_buffer), "%d. ", value);
    if (written < 0 || (size_t)written >= sizeof(index_buffer)) {
        return 0;
    }

    return append_path_text(buffer, buffer_size, offset, index_buffer);
}

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
    size_t node_buffer_size)
{
    const Node *current;
    int index = 1;
    size_t offset = 0;

    if (node_buffer == NULL || node_buffer_size == 0) {
        return 0;
    }
    node_buffer[0] = '\0';

    if (!append_path_text(node_buffer, node_buffer_size, &offset, "Node list:\n")) {
        return 0;
    }

    if (sim == NULL || sim->nodes == NULL) {
        return append_path_text(node_buffer, node_buffer_size, &offset, "  (empty)\n");
    }

    current = sim->nodes;
    while (current != NULL) {
        if (!append_path_text(node_buffer, node_buffer_size, &offset, "  ") ||
            !append_index_text(node_buffer, node_buffer_size, &offset, index) ||
            !append_path_text(node_buffer, node_buffer_size, &offset, current->name) ||
            !append_path_text(node_buffer, node_buffer_size, &offset, "\n")) {
            return 0;
        }
        index++;
        current = current->next;
    }

    return 1;
}

/**
 * @brief Formats the registered topic list into a text buffer.
 *
 * @param sim Simulator instance.
 * @param topic_buffer Output buffer for the formatted topic list.
 * @param topic_buffer_size Size of @p topic_buffer in bytes.
 * @return 1 if formatting succeeds, or 0 if the topic list is empty or invalid.
 */
int simulator_format_topics(
    const Simulator *sim,
    char *topic_buffer,
    size_t topic_buffer_size)
{
    const Topic *current;
    int index = 1;
    size_t offset = 0;

    if (topic_buffer == NULL || topic_buffer_size == 0) {
        return 0;
    }
    topic_buffer[0] = '\0';

    if (!append_path_text(topic_buffer, topic_buffer_size, &offset, "Topic list:\n")) {
        return 0;
    }

    if (sim == NULL || sim->topics == NULL) {
        return append_path_text(topic_buffer, topic_buffer_size, &offset, "  (empty)\n");
    }

    current = sim->topics;
    while (current != NULL) {
        const Publisher *publisher;
        const Subscriber *subscriber;
        int sub_index;

        if (!append_path_text(topic_buffer, topic_buffer_size, &offset, "  ") ||
            !append_index_text(topic_buffer, topic_buffer_size, &offset, index) ||
            !append_path_text(topic_buffer, topic_buffer_size, &offset, current->name) ||
            !append_path_text(topic_buffer, topic_buffer_size, &offset, "\n") ||
            !append_path_text(topic_buffer, topic_buffer_size, &offset, "    Publishers:\n")) {
            return 0;
        }

        publisher = current->publishers;
        if (publisher == NULL) {
            if (!append_path_text(topic_buffer, topic_buffer_size, &offset, "      (empty)\n")) {
                return 0;
            }
        } else {
            sub_index = 1;
            while (publisher != NULL) {
                if (!append_path_text(topic_buffer, topic_buffer_size, &offset, "      ") ||
                    !append_index_text(topic_buffer, topic_buffer_size, &offset, sub_index) ||
                    !append_path_text(topic_buffer, topic_buffer_size, &offset, publisher->node_name) ||
                    !append_path_text(topic_buffer, topic_buffer_size, &offset, "\n")) {
                    return 0;
                }
                sub_index++;
                publisher = publisher->next;
            }
        }

        if (!append_path_text(topic_buffer, topic_buffer_size, &offset, "    Subscribers:\n")) {
            return 0;
        }

        subscriber = current->subscribers;
        if (subscriber == NULL) {
            if (!append_path_text(topic_buffer, topic_buffer_size, &offset, "      (empty)\n")) {
                return 0;
            }
        } else {
            sub_index = 1;
            while (subscriber != NULL) {
                if (!append_path_text(topic_buffer, topic_buffer_size, &offset, "      ") ||
                    !append_index_text(topic_buffer, topic_buffer_size, &offset, sub_index) ||
                    !append_path_text(topic_buffer, topic_buffer_size, &offset, subscriber->node_name) ||
                    !append_path_text(topic_buffer, topic_buffer_size, &offset, "\n")) {
                    return 0;
                }
                sub_index++;
                subscriber = subscriber->next;
            }
        }

        index++;
        current = current->next;
    }

    return 1;
}

/**
 * @brief Formats the registered node list and topic list into a text buffer.
 *
 * @param sim Simulator instance.
 * @param list_buffer Output buffer for the formatted list text.
 * @param list_buffer_size Size of @p list_buffer in bytes.
 * @return 1 if formatting succeeds, or 0 if the buffer is invalid.
 */
int simulator_format_registered_lists(
    const Simulator *sim,
    char *list_buffer,
    size_t list_buffer_size)
{
    char node_buffer[1024];
    char topic_buffer[2048];
    size_t offset = 0;

    if (list_buffer == NULL || list_buffer_size == 0) {
        return 0;
    }
    list_buffer[0] = '\0';

    if (!simulator_format_nodes(sim, node_buffer, sizeof(node_buffer))) {
        return 0;
    }
    if (!simulator_format_topics(sim, topic_buffer, sizeof(topic_buffer))) {
        return 0;
    }

    if (!append_path_text(list_buffer, list_buffer_size, &offset, node_buffer) ||
        !append_path_text(list_buffer, list_buffer_size, &offset, topic_buffer)) {
        return 0;
    }

    return 1;
}

/**
 * @brief Formats the communication graph into a text buffer.
 *
 * @param sim Simulator instance.
 * @param graph_buffer Output buffer for the formatted graph text.
 * @param graph_buffer_size Size of @p graph_buffer in bytes.
 * @return 1 if formatting succeeds, or 0 if the graph is empty or invalid.
 */
int simulator_format_communication_graph(
    const Simulator *sim,
    char *graph_buffer,
    size_t graph_buffer_size)
{
    const Topic *topic;
    size_t offset = 0;
    int edge_count = 0;

    if (graph_buffer == NULL || graph_buffer_size == 0) {
        return 0;
    }
    graph_buffer[0] = '\0';

    if (sim == NULL || sim->topics == NULL) {
        return 0;
    }

    topic = sim->topics;
    while (topic != NULL) {
        const Publisher *publisher = topic->publishers;
        const Subscriber *subscriber = topic->subscribers;

        while (publisher != NULL) {
            if (!append_path_text(graph_buffer, graph_buffer_size, &offset, "  ") ||
                !append_path_text(graph_buffer, graph_buffer_size, &offset, publisher->node_name) ||
                !append_path_text(graph_buffer, graph_buffer_size, &offset, " -> ") ||
                !append_path_text(graph_buffer, graph_buffer_size, &offset, topic->name) ||
                !append_path_text(graph_buffer, graph_buffer_size, &offset, "\n")) {
                return 0;
            }
            edge_count++;
            publisher = publisher->next;
        }

        while (subscriber != NULL) {
            if (!append_path_text(graph_buffer, graph_buffer_size, &offset, "  ") ||
                !append_path_text(graph_buffer, graph_buffer_size, &offset, topic->name) ||
                !append_path_text(graph_buffer, graph_buffer_size, &offset, " -> ") ||
                !append_path_text(graph_buffer, graph_buffer_size, &offset, subscriber->node_name) ||
                !append_path_text(graph_buffer, graph_buffer_size, &offset, "\n")) {
                return 0;
            }
            edge_count++;
            subscriber = subscriber->next;
        }

        topic = topic->next;
    }

    return edge_count > 0;
}

/**
 * @brief Builds a directed communication path string between nodes.
 *
 * @param sim Simulator instance.
 * @param start_node_name Source node name.
 * @param target_node_name Destination node name.
 * @param path_buffer Output buffer for the formatted path string.
 * @param path_buffer_size Size of @p path_buffer in bytes.
 * @return 1 if a path is found and written successfully, or 0 otherwise.
 */
int simulator_format_path_between_nodes(
    const Simulator *sim,
    const char *start_node_name,
    const char *target_node_name,
    char *path_buffer,
    size_t path_buffer_size)
{
    const Node **nodes;
    int *queue;
    int *visited;
    int *previous;
    const char **via_topic;
    int node_count;
    int start_index;
    int target_index;
    int front = 0;
    int rear = 0;
    int found = 0;
    int index;

    if (path_buffer == NULL || path_buffer_size == 0) {
        return 0;
    }
    path_buffer[0] = '\0';

    if (sim == NULL || start_node_name == NULL || target_node_name == NULL) {
        return 0;
    }

    node_count = count_nodes(sim);
    if (node_count == 0) {
        return 0;
    }

    nodes = (const Node **)malloc(sizeof(Node *) * node_count);
    queue = (int *)malloc(sizeof(int) * node_count);
    visited = (int *)calloc((size_t)node_count, sizeof(int));
    previous = (int *)malloc(sizeof(int) * node_count);
    via_topic = (const char **)calloc((size_t)node_count, sizeof(char *));
    if (nodes == NULL || queue == NULL || visited == NULL || previous == NULL || via_topic == NULL) {
        free(nodes);
        free(queue);
        free(visited);
        free(previous);
        free(via_topic);
        return 0;
    }

    collect_nodes(sim, nodes);
    for (index = 0; index < node_count; index++) {
        previous[index] = -1;
    }

    start_index = find_node_index(nodes, node_count, start_node_name);
    target_index = find_node_index(nodes, node_count, target_node_name);
    if (start_index == -1 || target_index == -1) {
        free(nodes);
        free(queue);
        free(visited);
        free(previous);
        free(via_topic);
        return 0;
    }

    if (start_index == target_index) {
        size_t offset = 0;
        int ok = append_path_text(path_buffer, path_buffer_size, &offset, nodes[start_index]->name);
        free(nodes);
        free(queue);
        free(visited);
        free(previous);
        free(via_topic);
        return ok;
    }

    visited[start_index] = 1;
    queue[rear] = start_index;
    rear++;

    while (front < rear && !found) {
        int current_index = queue[front];
        const char *current_name = nodes[current_index]->name;
        const Topic *topic = sim->topics;

        front++;
        while (topic != NULL && !found) {
            if (find_publisher_in_topic(topic, current_name) != NULL) {
                const Subscriber *subscriber = topic->subscribers;

                while (subscriber != NULL) {
                    int next_index = find_node_index(nodes, node_count, subscriber->node_name);

                    if (next_index != -1 && !visited[next_index]) {
                        visited[next_index] = 1;
                        previous[next_index] = current_index;
                        via_topic[next_index] = topic->name;
                        queue[rear] = next_index;
                        rear++;

                        if (next_index == target_index) {
                            found = 1;
                            break;
                        }
                    }

                    subscriber = subscriber->next;
                }
            }

            topic = topic->next;
        }
    }

    if (found) {
        int *path;
        int path_length = 0;
        int current_index = target_index;
        size_t offset = 0;
        int ok = 1;

        path = (int *)malloc(sizeof(int) * node_count);
        if (path == NULL) {
            free(nodes);
            free(queue);
            free(visited);
            free(previous);
            free(via_topic);
            return 0;
        }

        while (current_index != -1) {
            path[path_length] = current_index;
            path_length++;
            current_index = previous[current_index];
        }

        for (index = path_length - 1; index >= 0 && ok; index--) {
            ok = append_path_text(path_buffer, path_buffer_size, &offset, nodes[path[index]]->name);
            if (index > 0 && ok) {
                ok = append_path_text(path_buffer, path_buffer_size, &offset, " -> ");
            }
            if (index > 0 && ok) {
                ok = append_path_text(path_buffer, path_buffer_size, &offset, via_topic[path[index - 1]]);
            }
            if (index > 0 && ok) {
                ok = append_path_text(path_buffer, path_buffer_size, &offset, " -> ");
            }
        }

        free(path);
        free(nodes);
        free(queue);
        free(visited);
        free(previous);
        free(via_topic);
        return ok;
    }

    free(nodes);
    free(queue);
    free(visited);
    free(previous);
    free(via_topic);
    return 0;
}
