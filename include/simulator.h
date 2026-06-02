#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stddef.h>

#define SIM_NAME_LENGTH 64

typedef struct Node {
    char name[SIM_NAME_LENGTH];
    struct Node *next;
} Node;

typedef struct Topic {
    char name[SIM_NAME_LENGTH];
    struct Topic *next;
} Topic;

typedef struct Simulator {
    Node *nodes;
    Topic *topics;
} Simulator;

void simulator_init(Simulator *sim);
void simulator_destroy(Simulator *sim);

int simulator_add_node(Simulator *sim, const char *name);
int simulator_add_topic(Simulator *sim, const char *name);

Node *simulator_find_node(const Simulator *sim, const char *name);
Topic *simulator_find_topic(const Simulator *sim, const char *name);

void simulator_print_nodes(const Simulator *sim);
void simulator_print_topics(const Simulator *sim);
void simulator_print_registered_lists(const Simulator *sim);

#endif
