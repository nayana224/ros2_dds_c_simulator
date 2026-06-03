/**
 * @file simulator.c
 * @brief ROS2 Pub/Sub C Simulator의 핵심 자료구조 구현.
 *
 * Node, Topic, Publisher, Subscriber는 단일 연결 리스트로 관리한다.
 * Message는 Topic 내부 priority queue에 저장하며, publish/subscribe 관계는
 * 그래프 간선처럼 해석하여 출력과 BFS 경로 탐색에 사용한다.
 */

#include "simulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Topic 내부 Publisher 리스트에서 특정 Node 이름을 탐색한다.
 *
 * @param topic 탐색할 Topic 포인터
 * @param node_name 찾을 Publisher Node 이름
 * @return 찾은 Publisher 포인터, 없으면 NULL
 */
static Publisher *find_publisher_in_topic(const Topic *topic, const char *node_name);

/**
 * @brief Topic 내부 Subscriber 리스트에서 특정 Node 이름을 탐색한다.
 *
 * @param topic 탐색할 Topic 포인터
 * @param node_name 찾을 Subscriber Node 이름
 * @return 찾은 Subscriber 포인터, 없으면 NULL
 */
static Subscriber *find_subscriber_in_topic(const Topic *topic, const char *node_name);

/**
 * @brief priority queue에 저장할 Message 노드를 생성한다.
 *
 * @param message 저장할 Message 문자열
 * @param priority Message 우선순위 값
 * @return 생성된 Message 포인터, 메모리 할당 실패 시 NULL
 */
static Message *create_message_node(const char *message, int priority);

/**
 * @brief Topic의 Message priority queue에 Message를 정렬 삽입한다.
 *
 * @param topic Message를 저장할 Topic 포인터
 * @param new_message 삽입할 Message 포인터
 */
static void enqueue_message_by_priority(Topic *topic, Message *new_message);

/**
 * @brief Simulator에 등록된 Node 개수를 센다.
 *
 * @param sim Simulator 포인터
 * @return 등록된 Node 개수
 */
static int count_nodes(const Simulator *sim);

/**
 * @brief Node 연결 리스트를 배열에 수집한다.
 *
 * @param sim Simulator 포인터
 * @param nodes Node 포인터를 저장할 배열
 */
static void collect_nodes(const Simulator *sim, const Node **nodes);

/**
 * @brief Node 포인터 배열에서 이름이 일치하는 Node의 인덱스를 찾는다.
 *
 * @param nodes Node 포인터 배열
 * @param node_count 배열에 저장된 Node 개수
 * @param name 찾을 Node 이름
 * @return 찾은 인덱스, 없으면 -1
 */
static int find_node_index(const Node **nodes, int node_count, const char *name);

/**
 * @brief Node와 Topic 이름이 유효한지 검사한다.
 *
 * @param name 검사할 이름 문자열
 * @return 유효하면 1, 유효하지 않으면 0
 */
static int is_valid_name(const char *name)
{
    return name != NULL && name[0] != '\0' && strlen(name) < SIM_NAME_LENGTH;
}

/**
 * @brief Message 문자열이 priority queue에 저장 가능한지 검사한다.
 *
 * @param message 검사할 Message 문자열
 * @return 유효하면 1, 유효하지 않으면 0
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
 * @brief Node 연결 리스트 전체를 순회하며 동적 할당 메모리를 해제한다.
 *
 * @param head 해제할 Node 연결 리스트의 head 포인터
 */
static void destroy_node_list(Node *head)
{
    Node *current = head;

    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
}

/**
 * @brief Topic 리스트와 각 Topic이 소유한 내부 리스트 및 Message queue를 해제한다.
 *
 * @param head 해제할 Topic 연결 리스트의 head 포인터
 */
static void destroy_topic_list(Topic *head)
{
    Topic *current = head;

    while (current != NULL) {
        Topic *next = current->next;
        Message *message = current->message_head;
        Publisher *publisher = current->publishers;
        Subscriber *subscriber = current->subscribers;

        while (message != NULL) {
            Message *message_next = message->next;
            free(message);
            message = message_next;
        }

        while (publisher != NULL) {
            Publisher *publisher_next = publisher->next;
            free(publisher);
            publisher = publisher_next;
        }

        while (subscriber != NULL) {
            Subscriber *subscriber_next = subscriber->next;
            free(subscriber);
            subscriber = subscriber_next;
        }

        free(current);
        current = next;
    }
}

/**
 * @brief Simulator 구조체를 빈 연결 리스트 상태로 초기화한다.
 *
 * @param sim 초기화할 Simulator 포인터
 */
void simulator_init(Simulator *sim)
{
    if (sim == NULL) {
        return;
    }

    sim->nodes = NULL;
    sim->topics = NULL;
}

/**
 * @brief Simulator가 소유한 모든 동적 할당 자료구조를 해제한다.
 *
 * @param sim 해제할 Simulator 포인터
 */
void simulator_destroy(Simulator *sim)
{
    if (sim == NULL) {
        return;
    }

    destroy_node_list(sim->nodes);
    destroy_topic_list(sim->topics);
    sim->nodes = NULL;
    sim->topics = NULL;
}

/**
 * @brief Node 연결 리스트에서 이름이 일치하는 Node를 순차 탐색한다.
 *
 * @param sim Simulator 포인터
 * @param name 찾을 Node 이름
 * @return 찾은 Node 포인터, 없으면 NULL
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
 * @brief Topic 연결 리스트에서 이름이 일치하는 Topic을 순차 탐색한다.
 *
 * @param sim Simulator 포인터
 * @param name 찾을 Topic 이름
 * @return 찾은 Topic 포인터, 없으면 NULL
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
 * @brief 공개 API에서 Topic 내부 Publisher 탐색 helper를 호출한다.
 *
 * @param topic 탐색할 Topic 포인터
 * @param node_name 찾을 Publisher Node 이름
 * @return 찾은 Publisher 포인터, 없으면 NULL
 */
Publisher *simulator_find_publisher(const Topic *topic, const char *node_name)
{
    return find_publisher_in_topic(topic, node_name);
}

/**
 * @brief 공개 API에서 Topic 내부 Subscriber 탐색 helper를 호출한다.
 *
 * @param topic 탐색할 Topic 포인터
 * @param node_name 찾을 Subscriber Node 이름
 * @return 찾은 Subscriber 포인터, 없으면 NULL
 */
Subscriber *simulator_find_subscriber(const Topic *topic, const char *node_name)
{
    return find_subscriber_in_topic(topic, node_name);
}

static Publisher *find_publisher_in_topic(const Topic *topic, const char *node_name)
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

static Subscriber *find_subscriber_in_topic(const Topic *topic, const char *node_name)
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
 * @brief 특정 Topic의 Publisher 연결 리스트를 출력한다.
 *
 * @param topic Publisher 목록을 출력할 Topic 포인터
 */
static void print_publisher_list(const Topic *topic)
{
    Publisher *current;
    int index = 1;

    printf("    Publishers:\n");
    if (topic == NULL || topic->publishers == NULL) {
        printf("      (empty)\n");
        return;
    }

    current = topic->publishers;
    while (current != NULL) {
        printf("      %d. %s\n", index, current->node_name);
        index++;
        current = current->next;
    }
}

/**
 * @brief 특정 Topic의 Subscriber 연결 리스트를 출력한다.
 *
 * @param topic Subscriber 목록을 출력할 Topic 포인터
 */
static void print_subscriber_list(const Topic *topic)
{
    Subscriber *current;
    int index = 1;

    printf("    Subscribers:\n");
    if (topic == NULL || topic->subscribers == NULL) {
        printf("      (empty)\n");
        return;
    }

    current = topic->subscribers;
    while (current != NULL) {
        printf("      %d. %s\n", index, current->node_name);
        index++;
        current = current->next;
    }
}

/**
 * @brief 새 Node를 Node 연결 리스트의 head에 삽입한다.
 *
 * @param sim Simulator 포인터
 * @param name 등록할 Node 이름
 * @return 등록 성공 시 1, 실패 시 0
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
 * @brief 새 Topic을 Topic 연결 리스트의 head에 삽입한다.
 *
 * @param sim Simulator 포인터
 * @param name 등록할 Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
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
 * @brief Node를 Topic의 Publisher 연결 리스트 head에 등록한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Publisher Node 이름
 * @param topic_name Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
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
 * @brief Node를 Topic의 Subscriber 연결 리스트 head에 등록한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Subscriber Node 이름
 * @param topic_name Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
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

/**
 * @brief Topic priority queue의 front Message를 제거한다.
 *
 * @param topic Message를 제거할 Topic 포인터
 * @return 제거된 Message 포인터, queue가 비어 있으면 NULL
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
 * @brief Subscriber 검증 후 Topic priority queue에서 Message를 수신한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Subscriber Node 이름
 * @param topic_name Topic 이름
 * @return 수신한 Message 포인터, 실패 시 NULL
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
 * @brief Publisher 검증 후 Message를 Topic priority queue에 발행한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Publisher Node 이름
 * @param topic_name Topic 이름
 * @param message 발행할 Message 데이터
 * @param priority Message 우선순위
 * @return 발행 성공 시 1, 실패 시 0
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

/**
 * @brief 등록된 Node 연결 리스트를 출력한다.
 *
 * @param sim Simulator 포인터
 */
void simulator_print_nodes(const Simulator *sim)
{
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

/**
 * @brief 등록된 Topic과 각 Topic의 Publisher/Subscriber 연결 리스트를 출력한다.
 *
 * @param sim Simulator 포인터
 */
void simulator_print_topics(const Simulator *sim)
{
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
        print_publisher_list(current);
        print_subscriber_list(current);
        index++;
        current = current->next;
    }
}

/**
 * @brief Node 목록과 Topic 관련 목록을 순서대로 출력한다.
 *
 * @param sim Simulator 포인터
 */
void simulator_print_registered_lists(const Simulator *sim)
{
    simulator_print_nodes(sim);
    simulator_print_topics(sim);
}

/**
 * @brief Publisher -> Topic -> Subscriber 관계를 그래프 간선 목록으로 출력한다.
 *
 * @param sim Simulator 포인터
 */
void simulator_print_communication_graph(const Simulator *sim)
{
    const Topic *topic;
    int edge_count = 0;

    printf("Communication graph:\n");
    if (sim == NULL || sim->topics == NULL) {
        printf("  (empty)\n");
        return;
    }

    topic = sim->topics;
    while (topic != NULL) {
        const Publisher *publisher = topic->publishers;
        const Subscriber *subscriber = topic->subscribers;

        while (publisher != NULL) {
            printf("  %s -> %s\n", publisher->node_name, topic->name);
            edge_count++;
            publisher = publisher->next;
        }

        while (subscriber != NULL) {
            printf("  %s -> %s\n", topic->name, subscriber->node_name);
            edge_count++;
            subscriber = subscriber->next;
        }

        topic = topic->next;
    }

    if (edge_count == 0) {
        printf("  (empty)\n");
    }
}

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

/**
 * @brief Node 간 메시지 전달 가능 경로를 BFS로 탐색하고 출력한다.
 *
 * 각 Node를 그래프 정점으로 보고, 어떤 Topic에서 현재 Node가 Publisher이고
 * 다른 Node가 Subscriber이면 현재 Node에서 해당 Subscriber Node로 가는 방향 간선이 있다고 본다.
 * BFS queue에는 Node 배열 인덱스를 저장하고, `previous`와 `via_topic` 배열로 경로를 복원한다.
 *
 * @param sim Simulator 포인터
 * @param start_node_name 시작 Node 이름
 * @param target_node_name 도착 Node 이름
 * @return 경로가 있으면 1, 없으면 0
 */
int simulator_print_path_between_nodes(const Simulator *sim, const char *start_node_name, const char *target_node_name)
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

    printf("Path search:\n");
    if (sim == NULL || start_node_name == NULL || target_node_name == NULL) {
        printf("  Path not found.\n");
        return 0;
    }

    node_count = count_nodes(sim);
    if (node_count == 0) {
        printf("  Path not found.\n");
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
        printf("  Path not found.\n");
        return 0;
    }

    collect_nodes(sim, nodes);
    for (index = 0; index < node_count; index++) {
        previous[index] = -1;
    }

    start_index = find_node_index(nodes, node_count, start_node_name);
    target_index = find_node_index(nodes, node_count, target_node_name);
    if (start_index == -1 || target_index == -1) {
        printf("  Path not found.\n");
        free(nodes);
        free(queue);
        free(visited);
        free(previous);
        free(via_topic);
        return 0;
    }

    if (start_index == target_index) {
        printf("  Path found: %s\n", nodes[start_index]->name);
        free(nodes);
        free(queue);
        free(visited);
        free(previous);
        free(via_topic);
        return 1;
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

        path = (int *)malloc(sizeof(int) * node_count);
        if (path == NULL) {
            printf("  Path not found.\n");
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

        printf("  Path found: ");
        for (index = path_length - 1; index >= 0; index--) {
            printf("%s", nodes[path[index]]->name);
            if (index > 0) {
                printf(" -> %s -> ", via_topic[path[index - 1]]);
            }
        }
        printf("\n");
        free(path);
    } else {
        printf("  Path not found.\n");
    }

    free(nodes);
    free(queue);
    free(visited);
    free(previous);
    free(via_topic);
    return found;
}
