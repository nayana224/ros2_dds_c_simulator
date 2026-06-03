/**
 * @file simulator.h
 * @brief ROS2 Pub/Sub 시뮬레이터의 기본 자료구조와 함수 선언.
 * 
 * 이 파일은 Node와 Topic을 연결 리스트로 관리하기 위한
 * 구조체와 함수 인터페이스를 정의한다.
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
 * @brief ROS2 Topic을 표현하는 연결 리스트 노드
 */
typedef struct Topic {
    char name[SIM_NAME_LENGTH];
    struct Publisher *publishers;
    struct Subscriber *subscribers;
    struct Topic *next;
} Topic;

/**
 * @brief ROS2 Publisher를 표현하는 연결 리스트 노드
 */
typedef struct Publisher {
    char node_name[SIM_NAME_LENGTH];
    struct Publisher *next;
} Publisher;

/**
 * @brief ROS2 Subscriber를 표현하는 연결 리스트 노드
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
 * @param sim Simulator 포인터
 * @param name 등록할 Node 이름
 * @return 성공 시 0, 실패 시 음수 값
 */
int simulator_add_node(Simulator *sim, const char *name);

/**
 * @brief 새로운 Topic을 시뮬레이터에 등록한다.
 * 
 * @param sim Simulator 포인터
 * @param name 등록할 Topic 이름
 * @return 성공 시 0, 실패 시 음수 값
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


int simulator_add_publisher(Simulator *sim, const char *node_name, const char *topic_name);
int simulator_add_subscriber(Simulator *sim, const char *node_name, const char *topic_name);
int simulator_publish_message(Simulator *sim, const char *node_name, const char *topic_name, const char *message, int priority);
Publisher *simulator_find_publisher(const Topic *topic, const char *node_name);
Subscriber *simulator_find_subscriber(const Topic *topic, const char *node_name);

#endif
