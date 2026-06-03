/**
 * @file simulator.h
 * @brief ROS2 Pub/Sub 개념을 C 자료구조로 단순화한 시뮬레이터의 공개 인터페이스.
 *
 * 이 헤더는 Node, Topic, Publisher, Subscriber, Message, Simulator 자료구조와
 * 시뮬레이터 조작 함수를 선언한다. 실제 ROS2, DDS, RTPS, socket은 사용하지 않고
 * 연결 리스트, priority queue, 그래프 탐색을 통해 통신 구조를 로컬 메모리에서 표현한다.
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stddef.h>

/**
 * @brief Node, Topic, Message 문자열을 저장하는 고정 길이 버퍼 크기.
 */
#define SIM_NAME_LENGTH 64

typedef struct Message Message;

/**
 * @brief ROS2 Node를 단일 연결 리스트 노드로 표현한 구조체.
 *
 * Simulator는 등록된 Node들을 `next` 포인터 기반 연결 리스트로 관리한다.
 */
typedef struct Node {
    char name[SIM_NAME_LENGTH]; /**< Node 이름 */
    struct Node *next; /**< 다음 Node를 가리키는 포인터 */
} Node;

/**
 * @brief ROS2 Topic을 단일 연결 리스트 노드로 표현한 구조체.
 *
 * 각 Topic은 Publisher 리스트, Subscriber 리스트, Message priority queue를 소유한다.
 * `message_head`는 가장 먼저 수신될 Message를 가리키고, `message_tail`은 마지막 Message를 가리킨다.
 */
typedef struct Topic {
    char name[SIM_NAME_LENGTH]; /**< Topic 이름 */
    struct Publisher *publishers; /**< Publisher 연결 리스트의 head 포인터 */
    struct Subscriber *subscribers; /**< Subscriber 연결 리스트의 head 포인터 */
    Message *message_head; /**< Message priority queue의 front 포인터 */
    Message *message_tail; /**< Message priority queue의 rear 포인터 */
    struct Topic *next; /**< 다음 Topic을 가리키는 포인터 */
} Topic;

/**
 * @brief 특정 Topic에 연결된 Publisher 정보를 표현하는 연결 리스트 노드.
 *
 * 실제 Node 포인터를 직접 저장하지 않고, 등록된 Publisher Node의 이름을 저장한다.
 */
typedef struct Publisher {
    char node_name[SIM_NAME_LENGTH]; /**< Publisher로 등록된 Node 이름 */
    struct Publisher *next; /**< 다음 Publisher를 가리키는 포인터 */
} Publisher;

/**
 * @brief 특정 Topic에 연결된 Subscriber 정보를 표현하는 연결 리스트 노드.
 *
 * 실제 Node 포인터를 직접 저장하지 않고, 등록된 Subscriber Node의 이름을 저장한다.
 */
typedef struct Subscriber {
    char node_name[SIM_NAME_LENGTH]; /**< Subscriber로 등록된 Node 이름 */
    struct Subscriber *next; /**< 다음 Subscriber를 가리키는 포인터 */
} Subscriber;

/**
 * @brief Topic 내부 Message priority queue의 원소를 표현하는 연결 리스트 노드.
 *
 * priority 값이 높은 Message가 queue의 앞쪽에 위치한다.
 * priority 값이 같으면 기존 발행 순서를 유지한다.
 */
typedef struct Message {
    char data[SIM_NAME_LENGTH]; /**< Message 데이터 문자열 */
    int priority; /**< Message 우선순위 값 */
    struct Message *next; /**< 다음 Message를 가리키는 포인터 */
} Message;

/**
 * @brief 시뮬레이터 전체 상태를 보관하는 최상위 구조체.
 *
 * 등록된 Node 리스트와 Topic 리스트의 head 포인터를 보관한다.
 * Topic 내부에는 Publisher, Subscriber, Message priority queue가 포함된다.
 */
typedef struct Simulator {
    Node *nodes; /**< 등록된 Node 연결 리스트의 head 포인터 */
    Topic *topics; /**< 등록된 Topic 연결 리스트의 head 포인터 */
} Simulator;

/**
 * @brief Simulator 구조체를 빈 상태로 초기화한다.
 *
 * @param sim 초기화할 Simulator 포인터
 */
void simulator_init(Simulator *sim);

/**
 * @brief Simulator가 동적 할당한 모든 자료구조 메모리를 해제한다.
 *
 * Node 리스트, Topic 리스트, 각 Topic 내부의 Publisher 리스트, Subscriber 리스트,
 * Message priority queue를 모두 순회하며 해제한다.
 *
 * @param sim 해제할 Simulator 포인터
 */
void simulator_destroy(Simulator *sim);

/**
 * @brief 새 Node를 Simulator에 등록한다.
 *
 * Node 이름은 NULL, 빈 문자열, `SIM_NAME_LENGTH` 이상 길이를 가질 수 없다.
 * 같은 이름의 Node는 중복 등록할 수 없다.
 *
 * @param sim Simulator 포인터
 * @param name 등록할 Node 이름
 * @return 등록 성공 시 1, 실패 시 0
 */
int simulator_add_node(Simulator *sim, const char *name);

/**
 * @brief 새 Topic을 Simulator에 등록한다.
 *
 * Topic 이름은 NULL, 빈 문자열, `SIM_NAME_LENGTH` 이상 길이를 가질 수 없다.
 * 같은 이름의 Topic은 중복 등록할 수 없다.
 *
 * @param sim Simulator 포인터
 * @param name 등록할 Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
 */
int simulator_add_topic(Simulator *sim, const char *name);

/**
 * @brief 이름으로 등록된 Node를 탐색한다.
 *
 * @param sim Simulator 포인터
 * @param name 찾을 Node 이름
 * @return 찾은 Node 포인터, 없으면 NULL
 */
Node *simulator_find_node(const Simulator *sim, const char *name);

/**
 * @brief 이름으로 등록된 Topic을 탐색한다.
 *
 * @param sim Simulator 포인터
 * @param name 찾을 Topic 이름
 * @return 찾은 Topic 포인터, 없으면 NULL
 */
Topic *simulator_find_topic(const Simulator *sim, const char *name);

/**
 * @brief 등록된 Node 목록을 출력한다.
 *
 * @param sim Simulator 포인터
 */
void simulator_print_nodes(const Simulator *sim);

/**
 * @brief 등록된 Topic 목록과 각 Topic의 Publisher/Subscriber 목록을 출력한다.
 *
 * @param sim Simulator 포인터
 */
void simulator_print_topics(const Simulator *sim);

/**
 * @brief 등록된 Node와 Topic 관련 목록을 모두 출력한다.
 *
 * @param sim Simulator 포인터
 */
void simulator_print_registered_lists(const Simulator *sim);

/**
 * @brief 등록된 publish/subscribe 관계를 Communication Graph 형태로 출력한다.
 *
 * 각 Topic을 기준으로 Publisher Node에서 Topic으로 향하는 간선과,
 * Topic에서 Subscriber Node로 향하는 간선을 출력한다.
 *
 * @param sim Simulator 포인터
 */
void simulator_print_communication_graph(const Simulator *sim);

/**
 * @brief 특정 Node에서 다른 Node까지 메시지 전달 경로를 BFS로 탐색하고 출력한다.
 *
 * Publisher Node -> Topic -> Subscriber Node 관계를 방향 그래프로 보고,
 * `start_node_name`에서 `target_node_name`까지 도달 가능한 경로가 있는지 탐색한다.
 *
 * @param sim Simulator 포인터
 * @param start_node_name 시작 Node 이름
 * @param target_node_name 도착 Node 이름
 * @return 경로가 존재하면 1, 존재하지 않거나 입력이 잘못되면 0
 */
int simulator_print_path_between_nodes(const Simulator *sim, const char *start_node_name, const char *target_node_name);

/**
 * @brief 특정 Node를 특정 Topic의 Publisher로 등록한다.
 *
 * Node와 Topic은 미리 Simulator에 등록되어 있어야 한다.
 * 같은 Topic에 같은 Node를 Publisher로 중복 등록할 수 없다.
 *
 * @param sim Simulator 포인터
 * @param node_name Publisher로 등록할 Node 이름
 * @param topic_name Publisher를 등록할 Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
 */
int simulator_add_publisher(Simulator *sim, const char *node_name, const char *topic_name);

/**
 * @brief 특정 Node를 특정 Topic의 Subscriber로 등록한다.
 *
 * Node와 Topic은 미리 Simulator에 등록되어 있어야 한다.
 * 같은 Topic에 같은 Node를 Subscriber로 중복 등록할 수 없다.
 *
 * @param sim Simulator 포인터
 * @param node_name Subscriber로 등록할 Node 이름
 * @param topic_name Subscriber를 등록할 Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
 */
int simulator_add_subscriber(Simulator *sim, const char *node_name, const char *topic_name);

/**
 * @brief Publisher Node가 특정 Topic에 Message를 발행한다.
 *
 * 발행에 성공하면 새 Message 노드를 생성하고 Topic 내부 priority queue에 정렬 삽입한다.
 * priority 값이 높은 Message가 먼저 수신되며, priority 값이 같으면 발행 순서를 유지한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Message를 발행할 Publisher Node 이름
 * @param topic_name Message를 발행할 Topic 이름
 * @param message 발행할 Message 데이터
 * @param priority Message 우선순위 값
 * @return 발행 및 queue 저장 성공 시 1, 실패 시 0
 */
int simulator_publish_message(Simulator *sim, const char *node_name, const char *topic_name, const char *message, int priority);

/**
 * @brief Subscriber Node가 구독 중인 Topic에서 Message를 수신한다.
 *
 * 수신에 성공하면 해당 Topic의 priority queue front에 있는 Message를 제거해 반환한다.
 *
 * @param sim Simulator 포인터
 * @param node_name Message를 수신할 Subscriber Node 이름
 * @param topic_name Message를 수신할 Topic 이름
 * @return 수신한 Message 포인터, 실패하거나 queue가 비어 있으면 NULL
 *
 * @warning 반환된 Message는 queue에서 분리된 동적 할당 객체이다.
 *          호출자는 사용 후 반드시 free() 해야 한다.
 */
Message *simulator_receive_message(Simulator *sim, const char *node_name, const char *topic_name);

/**
 * @brief Topic 내부 priority queue의 front Message를 제거하고 반환한다.
 *
 * @param topic Message를 꺼낼 Topic 포인터
 * @return 제거된 Message 포인터, queue가 비어 있으면 NULL
 *
 * @warning 반환된 Message는 queue에서 분리된 동적 할당 객체이다.
 *          호출자는 사용 후 반드시 free() 해야 한다.
 */
Message *simulator_dequeue_message(Topic *topic);

/**
 * @brief 특정 Topic에서 node_name에 해당하는 Publisher를 탐색한다.
 *
 * @param topic 탐색할 Topic 포인터
 * @param node_name 찾을 Publisher Node 이름
 * @return 찾은 Publisher 포인터, 없으면 NULL
 */
Publisher *simulator_find_publisher(const Topic *topic, const char *node_name);

/**
 * @brief 특정 Topic에서 node_name에 해당하는 Subscriber를 탐색한다.
 *
 * @param topic 탐색할 Topic 포인터
 * @param node_name 찾을 Subscriber Node 이름
 * @return 찾은 Subscriber 포인터, 없으면 NULL
 */
Subscriber *simulator_find_subscriber(const Topic *topic, const char *node_name);

#endif
