/**
 * @file simulator.h
 * @brief ROS2 Pub/Sub 시뮬레이터의 기본 자료구조와 함수 선언.
 *
 * 이 파일은 Node, Topic, Publisher, Subscriber를 연결 리스트 기반으로 관리하기 위한
 * 구조체와 함수 인터페이스를 정의한다.
 *
 * 각 Topic은 Publisher 리스트와 Subscriber 리스트를 소유하며,
 * 이를 통해 ROS2의 Topic 기반 Pub/Sub 관계를 단순화하여 표현한다.
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stddef.h>

#define SIM_NAME_LENGTH 64

/**
 * @brief ROS2 Node를 표현하는 연결 리스트 노드
 */
typedef struct Node {
    char name[SIM_NAME_LENGTH];
    struct Node *next; 
} Node;

/**
 * @brief ROS2 Topic을 표현하는 연결 리스트 노드.
 *
 * 각 Topic은 해당 Topic에 연결된 Publisher 목록과 Subscriber 목록을
 * 별도의 연결 리스트로 관리한다.
 */
typedef struct Topic {
    char name[SIM_NAME_LENGTH];
    struct Publisher *publishers;
    struct Subscriber *subscribers;
    struct Topic *next;
} Topic;

/**
 * @brief 특정 Topic에 연결된 Publisher 정보를 표현하는 연결 리스트 노드.
 *
 * Publisher는 실제 Node 구조체 포인터가 아니라 node_name 문자열을 저장한다.
 * 이 구조체는 Topic 내부 Publisher 리스트의 원소로 사용된다.
 */
typedef struct Publisher {
    char node_name[SIM_NAME_LENGTH];
    struct Publisher *next;
} Publisher;

/**
 * @brief 특정 Topic에 연결된 Subscriber 정보를 표현하는 연결 리스트 노드.
 *
 * Subscriber는 실제 Node 구조체 포인터가 아니라 node_name 문자열을 저장한다.
 * 이 구조체는 Topic 내부 Subscriber 리스트의 원소로 사용된다.
 */
typedef struct Subscriber {
    char node_name[SIM_NAME_LENGTH];
    struct Subscriber *next;
} Subscriber;

/**
 * @brief 시뮬레이터 전체 상태를 담는 구조체.
 * 
 * nodes는 등록된 Node 리스트의 head pointer이고,
 * topics는 등록된 Topic 리스트의 head pointer이다.
 */
typedef struct Simulator {
    Node *nodes; // Node's head pointer
    Topic *topics; // Topic's head pointer
} Simulator;





/**
 * @brief Simulator 구조체를 초기화한다.
 * 
 * @param sim 초기화할 Simulator 포인터
 */
void simulator_init(Simulator *sim);

/**
 * @brief Simulator가 동적으로 할당한 모든 메모리를 해체한다.
 * 
 * @param sim 해제할 Simulator 포인터
 */
void simulator_destroy(Simulator *sim);

/**
 * @brief 새로운 Node를 시뮬레이터에 등록한다.
 *
 * Node 이름은 NULL, 빈 문자열, SIM_NAME_LENGTH 이상의 길이를 가질 수 없다.
 * 같은 이름의 Node는 중복 등록할 수 없다.
 *
 * @param sim  Simulator 포인터
 * @param name 등록할 Node 이름
 * @return 등록 성공 시 1, 실패 시 0
 */
int simulator_add_node(Simulator *sim, const char *name);

/**
 * @brief 새로운 Topic을 시뮬레이터에 등록한다.
 *
 * Topic 이름은 NULL, 빈 문자열, SIM_NAME_LENGTH 이상의 길이를 가질 수 없다.
 * 같은 이름의 Topic은 중복 등록할 수 없다.
 * 새 Topic의 Publisher/Subscriber 리스트는 빈 상태로 초기화된다.
 *
 * @param sim  Simulator 포인터
 * @param name 등록할 Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
 */
int simulator_add_topic(Simulator *sim, const char *name);

/**
 * @brief 이름으로 Node를 탐색한다.
 * 
 * @param sim Simulator 포인터
 * @param name 찾을 Node 이름
 * @return 찾은 Node 포인터, 없으면 NULL
 */
Node *simulator_find_node(const Simulator *sim, const char *name);


/**
 * @brief 이름으로 Topic을 탐색한다.
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
 * @brief 등록된 Topic 목록을 출력한다.
 * 
 * @param sim Simulator 포인터
 */
void simulator_print_topics(const Simulator *sim);

/**
 * @brief 등록된 Node와 Topic 목록을 모두 출력한다.
 * 
 * @param sim Simulator 포인터
 */
void simulator_print_registered_lists(const Simulator *sim);

/**
 * @brief 특정 Node를 특정 Topic의 Publisher로 등록한다.
 *
 * Node와 Topic은 모두 이미 Simulator에 등록되어 있어야 한다.
 * 같은 Node를 같은 Topic의 Publisher로 중복 등록할 수 없다.
 *
 * @param sim        Simulator 포인터
 * @param node_name  Publisher로 등록할 Node 이름
 * @param topic_name Publisher를 등록할 Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
 */
int simulator_add_publisher(Simulator *sim, const char *node_name, const char *topic_name);

/**
 * @brief 특정 Node를 특정 Topic의 Subscriber로 등록한다.
 *
 * Node와 Topic은 모두 이미 Simulator에 등록되어 있어야 한다.
 * 같은 Node를 같은 Topic의 Subscriber로 중복 등록할 수 없다.
 *
 * @param sim        Simulator 포인터
 * @param node_name  Subscriber로 등록할 Node 이름
 * @param topic_name Subscriber를 등록할 Topic 이름
 * @return 등록 성공 시 1, 실패 시 0
 */
int simulator_add_subscriber(Simulator *sim, const char *node_name, const char *topic_name);

/**
 * @brief Publisher Node가 특정 Topic에 메시지를 발행한다.
 *
 * 현재 구현은 메시지를 실제 Queue에 저장하지 않고,
 * 발행 가능 조건을 검사한 뒤 메시지 정보를 출력한다.
 *
 * 발행이 성공하려면 Node와 Topic이 이미 등록되어 있어야 하며,
 * 해당 Node가 해당 Topic의 Publisher로 등록되어 있어야 한다.
 *
 * @param sim        Simulator 포인터
 * @param node_name  메시지를 발행할 Publisher Node 이름
 * @param topic_name 메시지를 발행할 Topic 이름
 * @param message    발행할 메시지 문자열
 * @param priority   메시지 우선순위 값
 * @return 발행 성공 시 1, 실패 시 0
 */
int simulator_publish_message(Simulator *sim, const char *node_name, const char *topic_name, const char *message, int priority);

/**
 * @brief 특정 Topic에서 node_name에 해당하는 Publisher를 탐색한다.
 *
 * @param topic     탐색할 Topic 포인터
 * @param node_name 찾을 Publisher Node 이름
 * @return 찾은 Publisher 포인터, 없으면 NULL
 */
Publisher *simulator_find_publisher(const Topic *topic, const char *node_name);

/**
 * @brief 특정 Topic에서 node_name에 해당하는 Subscriber를 탐색한다.
 *
 * @param topic     탐색할 Topic 포인터
 * @param node_name 찾을 Subscriber Node 이름
 * @return 찾은 Subscriber 포인터, 없으면 NULL
 */
Subscriber *simulator_find_subscriber(const Topic *topic, const char *node_name);

#endif
