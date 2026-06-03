/**
 * @file cli.c
 * @brief Command-driven CLI for the ROS2 Pub/Sub C simulator.
 *
 * This file reads command input and dispatches Node registration,
 * Topic registration, Publisher registration, Subscriber registration,
 * Message Publish, and registered-list printing.
 *
 * The current Message Publish behavior validates publisher access
 * and stores the published message in the Topic's internal priority queue.
 * Receive Message validates subscriber access and dequeues one message.
 */

#include "cli.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLI_LINE_LENGTH 256
#define CLI_MAX_TOKENS 8

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

static int split_command(char *line, char *tokens[], int max_tokens)
{
    int count = 0;
    char *current = line;

    while (*current != '\0') {
        while (isspace((unsigned char)*current)) {
            current++;
        }

        if (*current == '\0') {
            break;
        }

        if (count >= max_tokens) {
            return count;
        }

        if (*current == '"') {
            current++;
            tokens[count] = current;
            while (*current != '\0' && *current != '"') {
                current++;
            }
        } else {
            tokens[count] = current;
            while (*current != '\0' && !isspace((unsigned char)*current)) {
                current++;
            }
        }

        if (*current != '\0') {
            *current = '\0';
            current++;
        }

        count++;
    }

    return count;
}

static int parse_int_arg(const char *text, int *value)
{
    char *endptr;
    long parsed;

    if (text == NULL || value == NULL || text[0] == '\0') {
        return 0;
    }

    parsed = strtol(text, &endptr, 10);
    if (*endptr != '\0' || parsed < INT_MIN || parsed > INT_MAX) {
        return 0;
    }

    *value = (int)parsed;
    return 1;
}

/**
 * @brief Print the available CLI commands.
 */
static void print_help(void)
{
    printf("Commands:\n");
    printf("  add_node <node>\n");
    printf("  add_topic <topic>\n");
    printf("  add_publisher <node> <topic>\n");
    printf("  add_subscriber <node> <topic>\n");
    printf("  publish <publisher_node> <topic> <message> <priority>\n");
    printf("  receive <subscriber_node> <topic>\n");
    printf("  list\n");
    printf("  graph\n");
    printf("  search <start_node> <target_node>\n");
    printf("  help\n");
    printf("  exit\n");
    printf("Use quotes for messages with spaces, for example: publish lidar_node /scan \"range data\" 5\n");
}


/**
 * @brief Handle Node registration from a command.
 *
 * @param sim Simulator instance used for registration.
 */
static void handle_add_node(Simulator *sim, const char *name)
{
    if (simulator_add_node(sim, name)) {
        printf("Node '%s' registered successfully.\n", name);
    } else {
        printf("Failed to register node. Check for empty, too long, or duplicate name.\n");
    }
}


/**
 * @brief Handle Topic registration from a command.
 *
 * @param sim Simulator instance used for registration.
 */
static void handle_add_topic(Simulator *sim, const char *name)
{
    if (simulator_add_topic(sim, name)) {
        printf("Topic '%s' registered successfully.\n", name);
    } else {
        printf("Failed to register topic. Check for empty, too long, or duplicate name.\n");
    }
}


/**
 * @brief Handle Publisher registration from a command.
 *
 * @param sim Simulator instance used for registration.
 */
static void handle_add_publisher(Simulator *sim, const char *node_name, const char *topic_name)
{
    if (simulator_add_publisher(sim, node_name, topic_name)) {
        printf("Publisher '%s' -> '%s' registered successfully.\n", node_name, topic_name);
    } else {
        printf("Failed to register publisher. Check node/topic existence or duplicate link.\n");
    }
}


/**
 * @brief Handle Subscriber registration from a command.
 *
 * @param sim Simulator instance used for registration.
 */
static void handle_add_subscriber(Simulator *sim, const char *node_name, const char *topic_name)
{
    if (simulator_add_subscriber(sim, node_name, topic_name)) {
        printf("Subscriber '%s' -> '%s' registered successfully.\n", node_name, topic_name);
    } else {
        printf("Failed to register subscriber. Check node/topic existence or duplicate link.\n");
    }
}


/**
 * @brief Handle message publishing from a command.
 *
 * @param sim Simulator instance used for publishing.
 */
static void handle_publish_message(Simulator *sim, const char *node_name, const char *topic_name, const char *message, const char *priority_text)
{
    int priority;

    if (!parse_int_arg(priority_text, &priority)) {
        printf("Failed to publish message. Invalid priority.\n");
        return;
    }

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
static void handle_receive_message(Simulator *sim, const char *node_name, const char *topic_name)
{
    Message *message;

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


static void print_usage_error(void)
{
    printf("Invalid command format. Type 'help' to see available commands.\n");
}


/**
 * @brief Run the command-driven CLI loop for the simulator.
 *
 * @param sim Simulator instance used by the CLI.
 */
void cli_run(Simulator *sim) 
{
    char line[CLI_LINE_LENGTH];
    char *tokens[CLI_MAX_TOKENS];
    int token_count;

    printf("=== ROS2 Pub/Sub C Simulator ===\n");
    print_help();
    for (;;) {
        printf("\nros2-sim> ");
        read_name_input(line, sizeof(line));
        token_count = split_command(line, tokens, CLI_MAX_TOKENS);

        if (token_count == 0) {
            continue;
        }

        if (strcmp(tokens[0], "help") == 0) {
            print_help();
        } else if (strcmp(tokens[0], "exit") == 0 || strcmp(tokens[0], "quit") == 0) {
            break;
        } else if (strcmp(tokens[0], "add_node") == 0) {
            if (token_count == 2) {
                handle_add_node(sim, tokens[1]);
            } else {
                print_usage_error();
            }
        } else if (strcmp(tokens[0], "add_topic") == 0) {
            if (token_count == 2) {
                handle_add_topic(sim, tokens[1]);
            } else {
                print_usage_error();
            }
        } else if (strcmp(tokens[0], "add_publisher") == 0) {
            if (token_count == 3) {
                handle_add_publisher(sim, tokens[1], tokens[2]);
            } else {
                print_usage_error();
            }
        } else if (strcmp(tokens[0], "add_subscriber") == 0) {
            if (token_count == 3) {
                handle_add_subscriber(sim, tokens[1], tokens[2]);
            } else {
                print_usage_error();
            }
        } else if (strcmp(tokens[0], "publish") == 0) {
            if (token_count == 5) {
                handle_publish_message(sim, tokens[1], tokens[2], tokens[3], tokens[4]);
            } else {
                print_usage_error();
            }
        } else if (strcmp(tokens[0], "receive") == 0) {
            if (token_count == 3) {
                handle_receive_message(sim, tokens[1], tokens[2]);
            } else {
                print_usage_error();
            }
        } else if (strcmp(tokens[0], "list") == 0) {
            simulator_print_registered_lists(sim);
        } else if (strcmp(tokens[0], "graph") == 0) {
            simulator_print_communication_graph(sim);
        } else if (strcmp(tokens[0], "search") == 0) {
            if (token_count == 3) {
                simulator_print_path_between_nodes(sim, tokens[1], tokens[2]);
            } else {
                print_usage_error();
            }
        } else {
            printf("Unknown command. Type 'help' to see available commands.\n");
        }
    }
}
