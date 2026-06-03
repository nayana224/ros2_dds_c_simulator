/**
 * @file cli.c
 * @brief Menu-driven CLI for the ROS2 Pub/Sub C simulator.
 *
 * This file reads menu input and dispatches Node registration,
 * Topic registration, Publisher registration, Subscriber registration,
 * Message Publish, and registered-list printing.
 *
 * The current Message Publish behavior validates publisher access
 * and stores the published message in the Topic's internal priority queue.
 * Receive Message validates subscriber access and dequeues one message.
 */

#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief Read a string from stdin and strip the trailing newline.
 *
 * @param buffer Destination buffer for the input string.
 * @param size   Total size of buffer.
 */
static void read_name_input(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';
}

static void discard_line_remainder(void)
{
    int ch;

    do {
        ch = getchar();
    } while (ch != '\n' && ch != EOF);
}

/**
 * @brief Print the available menu options.
 */
static void print_menu(void) 
{
    printf("\n=== ROS2 Pub/Sub C Simulator ===\n");
    printf("1. Add Node\n");
    printf("2. Add Topic\n");
    printf("3. Add Publisher\n");
    printf("4. Add Subscriber\n");
    printf("5. Publish Message\n");
    printf("6. Receive Message\n");
    printf("7. Print Registered Lists\n");
    printf("8. Print Communication Graph (not implemented)\n");
    printf("9. Search Path Between Nodes (not implemented)\n");
    printf("0. Exit\n");
    printf("Select: ");
}


/**
 * @brief Handle Node registration from the menu.
 *
 * @param sim Simulator instance used for registration.
 */
static void handle_add_node(Simulator *sim) 
{
    char name[SIM_NAME_LENGTH];

    printf("Enter node name: ");
    read_name_input(name, sizeof(name));

    if (simulator_add_node(sim, name)) {
        printf("Node '%s' registered successfully.\n", name);
    } else {
        printf("Failed to register node. Check for empty, too long, or duplicate name.\n");
    }
}


/**
 * @brief Handle Topic registration from the menu.
 *
 * @param sim Simulator instance used for registration.
 */
static void handle_add_topic(Simulator *sim) 
{
    char name[SIM_NAME_LENGTH];

    printf("Enter topic name: ");
    read_name_input(name, sizeof(name));

    if (simulator_add_topic(sim, name)) {
        printf("Topic '%s' registered successfully.\n", name);
    } else {
        printf("Failed to register topic. Check for empty, too long, or duplicate name.\n");
    }
}


/**
 * @brief Handle Publisher registration from the menu.
 *
 * @param sim Simulator instance used for registration.
 */
static void handle_add_publisher(Simulator *sim) 
{
    char node_name[SIM_NAME_LENGTH];
    char topic_name[SIM_NAME_LENGTH];

    printf("Enter node name: ");
    read_name_input(node_name, sizeof(node_name));
    printf("Enter topic name: ");
    read_name_input(topic_name, sizeof(topic_name));

    if (simulator_add_publisher(sim, node_name, topic_name)) {
        printf("Publisher '%s' -> '%s' registered successfully.\n", node_name, topic_name);
    } else {
        printf("Failed to register publisher. Check node/topic existence or duplicate link.\n");
    }
}


/**
 * @brief Handle Subscriber registration from the menu.
 *
 * @param sim Simulator instance used for registration.
 */
static void handle_add_subscriber(Simulator *sim) 
{
    char node_name[SIM_NAME_LENGTH];
    char topic_name[SIM_NAME_LENGTH];

    printf("Enter node name: ");
    read_name_input(node_name, sizeof(node_name));
    printf("Enter topic name: ");
    read_name_input(topic_name, sizeof(topic_name));

    if (simulator_add_subscriber(sim, node_name, topic_name)) {
        printf("Subscriber '%s' -> '%s' registered successfully.\n", node_name, topic_name);
    } else {
        printf("Failed to register subscriber. Check node/topic existence or duplicate link.\n");
    }
}


/**
 * @brief Handle message publishing from the menu.
 *
 * @param sim Simulator instance used for publishing.
 */
static void handle_publish_message(Simulator *sim) 
{
    char node_name[SIM_NAME_LENGTH];
    char topic_name[SIM_NAME_LENGTH];
    char message[SIM_NAME_LENGTH];
    int priority;

    printf("Enter node name: ");
    read_name_input(node_name, sizeof(node_name));
    printf("Enter topic name: ");
    read_name_input(topic_name, sizeof(topic_name));
    printf("Enter message: ");
    read_name_input(message, sizeof(message));
    printf("Enter priority: ");
    if (scanf("%d", &priority) != 1) {
        discard_line_remainder();
        printf("Failed to publish message. Invalid priority.\n");
        return;
    }
    discard_line_remainder();

    if (simulator_publish_message(sim, node_name, topic_name, message, priority)) {
        printf("Message published:\n");
        printf("Topic: %s\n", topic_name);
        printf("Publisher: %s\n", node_name);
        printf("Data: %s\n", message);
        printf("Priority: %d\n", priority);
    } else {
        printf("Failed to publish message. Check node/topic/publisher existence or empty message.\n");
    }
}


/**
 * @brief 메뉴에서 Message 수신을 처리한다.
 *
 * Subscriber Node 이름과 Topic 이름을 입력받고,
 * 해당 Topic의 priority queue에서 우선순위가 가장 높은 Message를 하나 수신한다.
 *
 * @param sim Simulator instance used for receiving.
 */
static void handle_receive_message(Simulator *sim)
{
    char node_name[SIM_NAME_LENGTH];
    char topic_name[SIM_NAME_LENGTH];
    Message *message;

    printf("Enter node name: ");
    read_name_input(node_name, sizeof(node_name));
    printf("Enter topic name: ");
    read_name_input(topic_name, sizeof(topic_name));

    message = simulator_receive_message(sim, node_name, topic_name);
    if (message == NULL) {
        printf("Failed to receive message. Check node/topic/subscriber existence or empty queue.\n");
        return;
    }

    printf("Message received:\n");
    printf("Topic: %s\n", topic_name);
    printf("Subscriber: %s\n", node_name);
    printf("Data: %s\n", message->data);
    printf("Priority: %d\n", message->priority);
    free(message);
}


/**
 * @brief Run the CLI loop for the simulator.
 *
 * @param sim Simulator instance used by the CLI.
 */
void cli_run(Simulator *sim) 
{
    int choice;

    for (;;) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        /*
         * Remove the trailing newline left in the input buffer by scanf("%d", ...).
         * If this is not removed, a later fgets() call may read an empty string.
         */
        discard_line_remainder();

        switch (choice) {
            case 1:
                handle_add_node(sim);
                break;
            case 2:
                handle_add_topic(sim);
                break;
            case 3:
                handle_add_publisher(sim);
                break;
            case 4:
                handle_add_subscriber(sim);
                break;
            case 5:
                handle_publish_message(sim);
                break;
            case 6:
                handle_receive_message(sim);
                break;
            case 7:
                simulator_print_registered_lists(sim);
                break;
            case 0:
                return;
            case 8:
            case 9:
                printf("This feature is intentionally not implemented in this step.\n");
                break;
            default:
                printf("Unknown menu option.\n");
                break;
        }
    }
}
