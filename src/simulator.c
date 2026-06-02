#include "simulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_valid_name(const char *name) {
    return name != NULL && name[0] != '\0' && strlen(name) < SIM_NAME_LENGTH;
}

static void destroy_node_list(Node *head) {
    Node *current = head;

    /* Free every node in the linked list one by one. */
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
}

static void destroy_topic_list(Topic *head) {
    Topic *current = head;

    /* Free every topic in the linked list one by one. */
    while (current != NULL) {
        Topic *next = current->next;
        free(current);
        current = next;
    }
}

void simulator_init(Simulator *sim) {
    if (sim == NULL) {
        return;
    }

    sim->nodes = NULL;
    sim->topics = NULL;
}

void simulator_destroy(Simulator *sim) {
    if (sim == NULL) {
        return;
    }

    destroy_node_list(sim->nodes);
    destroy_topic_list(sim->topics);
    sim->nodes = NULL;
    sim->topics = NULL;
}

Node *simulator_find_node(const Simulator *sim, const char *name) {
    Node *current;

    if (sim == NULL || name == NULL) {
        return NULL;
    }

    current = sim->nodes;

    /* Traverse the node linked list until the requested name is found. */
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

Topic *simulator_find_topic(const Simulator *sim, const char *name) {
    Topic *current;

    if (sim == NULL || name == NULL) {
        return NULL;
    }

    current = sim->topics;

    /* Traverse the topic linked list until the requested name is found. */
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

int simulator_add_node(Simulator *sim, const char *name) {
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

    /* Insert the new node at the head of the linked list in O(1) time. */
    strcpy(new_node->name, name);
    new_node->next = sim->nodes;
    sim->nodes = new_node;

    return 1;
}

int simulator_add_topic(Simulator *sim, const char *name) {
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

    /* Insert the new topic at the head of the linked list in O(1) time. */
    strcpy(new_topic->name, name);
    new_topic->next = sim->topics;
    sim->topics = new_topic;

    return 1;
}

void simulator_print_nodes(const Simulator *sim) {
    const Node *current;
    int index = 1;

    printf("Node list:\n");
    if (sim == NULL || sim->nodes == NULL) {
        printf("  (empty)\n");
        return;
    }

    current = sim->nodes;
    while (current != NULL) {
        printf("  %d. %s\n", index, current->name);
        index++;
        current = current->next;
    }
}

void simulator_print_topics(const Simulator *sim) {
    const Topic *current;
    int index = 1;

    printf("Topic list:\n");
    if (sim == NULL || sim->topics == NULL) {
        printf("  (empty)\n");
        return;
    }

    current = sim->topics;
    while (current != NULL) {
        printf("  %d. %s\n", index, current->name);
        index++;
        current = current->next;
    }
}

void simulator_print_registered_lists(const Simulator *sim) {
    simulator_print_nodes(sim);
    simulator_print_topics(sim);
}
