/**
 * @file cli.c
 * @brief Command-line interface implementation for the pub/sub simulator.
 *
 * This file parses user commands, validates basic CLI input, and dispatches
 * operations to the simulator core.
 */

#include "cli.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Maximum length of a single input line read by the CLI.
 */
#define CLI_LINE_LENGTH 256

/**
 * @brief Maximum number of tokens parsed from one command line.
 */
#define CLI_MAX_TOKENS 8

/**
 * @brief Reads one line from standard input and removes trailing newlines.
 *
 * @param buffer Output buffer for the line text.
 * @param size Size of @p buffer in bytes.
 */
static void read_name_input(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';
}

/**
 * @brief Splits a command line into whitespace-delimited tokens.
 *
 * Double-quoted text is treated as a single token so that messages containing
 * spaces can be passed as one argument.
 *
 * @param line Input line to tokenize. The buffer is modified in place.
 * @param tokens Output array of token pointers.
 * @param max_tokens Maximum number of token pointers that can be stored.
 * @return Number of tokens parsed from the line.
 */
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

/**
 * @brief Parses a decimal integer argument.
 *
 * @param text Input text to parse.
 * @param value Output location for the parsed integer.
 * @return 1 if parsing succeeds, or 0 if the input is invalid.
 */
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
 * @brief Prints the list of supported CLI commands.
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
 * @brief Handles the `add_node` command.
 *
 * @param sim Simulator instance.
 * @param name Node name to register.
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
 * @brief Handles the `add_topic` command.
 *
 * @param sim Simulator instance.
 * @param name Topic name to register.
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
 * @brief Handles the `add_publisher` command.
 *
 * @param sim Simulator instance.
 * @param node_name Node name to register as a publisher.
 * @param topic_name Topic name to attach the publisher to.
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
 * @brief Handles the `add_subscriber` command.
 *
 * @param sim Simulator instance.
 * @param node_name Node name to register as a subscriber.
 * @param topic_name Topic name to attach the subscriber to.
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
 * @brief Handles the `publish` command.
 *
 * This function parses the priority argument and forwards the publish request
 * to the simulator core.
 *
 * @param sim Simulator instance.
 * @param node_name Publisher node name.
 * @param topic_name Target topic name.
 * @param message Message payload.
 * @param priority_text Priority value as CLI text.
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
 * @brief Handles the `receive` command.
 *
 * The CLI prints the received message and releases the returned heap-allocated
 * Message object after use.
 *
 * @param sim Simulator instance.
 * @param node_name Subscriber node name.
 * @param topic_name Target topic name.
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

/**
 * @brief Prints a generic usage error for malformed commands.
 */
static void print_usage_error(void)
{
    printf("Invalid command format. Type 'help' to see available commands.\n");
}

/**
 * @brief Handles the `list` command.
 *
 * The simulator core formats the registered node and topic lists, and the CLI
 * prints the resulting text.
 *
 * @param sim Simulator instance.
 */
static void handle_print_registered_lists(Simulator *sim)
{
    char list_buffer[CLI_LINE_LENGTH * 16];

    if (simulator_format_registered_lists(sim, list_buffer, sizeof(list_buffer))) {
        printf("%s", list_buffer);
    } else {
        printf("Node list:\n");
        printf("  (empty)\n");
        printf("Topic list:\n");
        printf("  (empty)\n");
    }
}

/**
 * @brief Handles the `graph` command.
 *
 * The simulator core formats the graph content, and the CLI prints the
 * user-facing heading and body.
 *
 * @param sim Simulator instance.
 */
static void handle_print_graph(Simulator *sim)
{
    char graph_buffer[CLI_LINE_LENGTH * 8];

    printf("Communication graph:\n");
    if (simulator_format_communication_graph(sim, graph_buffer, sizeof(graph_buffer))) {
        printf("%s", graph_buffer);
    } else {
        printf("  (empty)\n");
    }
}

/**
 * @brief Handles the `search` command.
 *
 * The simulator core computes the path string, and the CLI is responsible for
 * printing the user-facing result.
 *
 * @param sim Simulator instance.
 * @param start_node_name Source node name.
 * @param target_node_name Destination node name.
 */
static void handle_search_path(Simulator *sim, const char *start_node_name, const char *target_node_name)
{
    char path_buffer[CLI_LINE_LENGTH * 2];

    printf("Path search:\n");
    if (simulator_format_path_between_nodes(
            sim,
            start_node_name,
            target_node_name,
            path_buffer,
            sizeof(path_buffer))) {
        printf("  Path found: %s\n", path_buffer);
    } else {
        printf("  Path not found.\n");
    }
}

/**
 * @brief Runs the interactive command loop.
 *
 * The CLI reads commands from standard input, tokenizes them, dispatches the
 * corresponding simulator operations, and prints results to standard output.
 * The loop exits when the user enters `exit` or `quit`.
 *
 * @param sim Simulator instance controlled by the CLI.
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
            handle_print_registered_lists(sim);
        } else if (strcmp(tokens[0], "graph") == 0) {
            handle_print_graph(sim);
        } else if (strcmp(tokens[0], "search") == 0) {
            if (token_count == 3) {
                handle_search_path(sim, tokens[1], tokens[2]);
            } else {
                print_usage_error();
            }
        } else {
            printf("Unknown command. Type 'help' to see available commands.\n");
        }
    }
}
