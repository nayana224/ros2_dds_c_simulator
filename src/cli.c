/**
 * @file cli.c
 * @brief ROS2 Pub/Sub C Simulator의 명령어 기반 CLI 구현.
 *
 * 사용자가 입력한 한 줄 명령어를 파싱하여 Simulator API를 호출한다.
 * 실제 ROS2 명령어와 동일하지는 않지만, `add_node`, `publish`, `receive`, `graph`,
 * `search` 같은 명령어를 통해 Pub/Sub 시뮬레이터를 조작할 수 있게 한다.
 */

#include "cli.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief CLI 한 줄 입력 버퍼의 최대 길이.
 */
#define CLI_LINE_LENGTH 256

/**
 * @brief 한 명령어에서 분리할 수 있는 최대 토큰 수.
 */
#define CLI_MAX_TOKENS 8

/**
 * @brief 표준 입력에서 문자열 한 줄을 읽고 줄바꿈 문자를 제거한다.
 *
 * @param buffer 입력 문자열을 저장할 버퍼
 * @param size 버퍼 전체 크기
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
 * @brief 명령어 한 줄을 공백 기준 토큰 배열로 분리한다.
 *
 * 큰따옴표로 감싼 문자열은 공백을 포함해 하나의 토큰으로 처리한다.
 * 예를 들어 `"range data"`는 하나의 message 인자로 분리된다.
 *
 * @param line 파싱할 명령어 문자열
 * @param tokens 분리된 토큰 포인터를 저장할 배열
 * @param max_tokens tokens 배열의 최대 원소 수
 * @return 분리된 토큰 개수
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
 * @brief 문자열 인자를 int 값으로 변환한다.
 *
 * priority 입력값이 정수 범위를 벗어나거나 숫자가 아니면 실패한다.
 *
 * @param text 정수로 변환할 문자열
 * @param value 변환된 정수를 저장할 포인터
 * @return 변환 성공 시 1, 실패 시 0
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
 * @brief CLI에서 사용할 수 있는 명령어 목록을 출력한다.
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
 * @brief `add_node` 명령어를 처리한다.
 *
 * @param sim Simulator 포인터
 * @param name 등록할 Node 이름
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
 * @brief `add_topic` 명령어를 처리한다.
 *
 * @param sim Simulator 포인터
 * @param name 등록할 Topic 이름
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
 * @brief `add_publisher` 명령어를 처리한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Publisher로 등록할 Node 이름
 * @param topic_name Publisher를 등록할 Topic 이름
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
 * @brief `add_subscriber` 명령어를 처리한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Subscriber로 등록할 Node 이름
 * @param topic_name Subscriber를 등록할 Topic 이름
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
 * @brief `publish` 명령어를 처리한다.
 *
 * priority 문자열을 정수로 변환한 뒤, Publisher 권한 검증과 Message priority queue 삽입은
 * Simulator API에 위임한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Message를 발행할 Publisher Node 이름
 * @param topic_name Message를 발행할 Topic 이름
 * @param message 발행할 Message 데이터
 * @param priority_text priority 정수 문자열
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
 * @brief `receive` 명령어를 처리한다.
 *
 * Subscriber 권한 검증과 priority queue front 제거는 Simulator API에 위임한다.
 * 수신한 Message는 CLI에서 출력한 뒤 `free()`로 해제한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Message를 수신할 Subscriber Node 이름
 * @param topic_name Message를 수신할 Topic 이름
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
 * @brief 잘못된 명령어 형식에 대한 안내 메시지를 출력한다.
 */
static void print_usage_error(void)
{
    printf("Invalid command format. Type 'help' to see available commands.\n");
}

/**
 * @brief 명령어 기반 CLI 루프를 실행한다.
 *
 * 한 줄 명령어를 읽고 토큰으로 분리한 뒤, 첫 번째 토큰을 명령어 이름으로 해석한다.
 * 각 명령어는 Simulator API를 호출하여 Node/Topic 등록, 메시지 발행/수신,
 * 그래프 출력, BFS 경로 탐색을 수행한다.
 *
 * @param sim CLI에서 사용할 Simulator 포인터
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
