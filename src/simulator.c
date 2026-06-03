#include "simulator.h"

#include <stdio.h> // printf
#include <stdlib.h> // malloc, free
#include <string.h> // strlen, strcmp, strcpy

/**
 * @brief 특정 Topic 안에서 node_name에 해당하는 Publisher를 탐색한다.
 *
 * simulator.c 내부에서만 사용하는 보조 함수이다.
 *
 * @param topic     탐색할 Topic 포인터
 * @param node_name 찾을 Publisher Node 이름
 * @return 찾은 Publisher 포인터, 없으면 NULL
 */
static Publisher *find_publisher_in_topic(const Topic *topic, const char *node_name);

/**
 * @brief 특정 Topic 안에서 node_name에 해당하는 Subscriber를 탐색한다.
 *
 * simulator.c 내부에서만 사용하는 보조 함수이다.
 *
 * @param topic     탐색할 Topic 포인터
 * @param node_name 찾을 Subscriber Node 이름
 * @return 찾은 Subscriber 포인터, 없으면 NULL
 */
static Subscriber *find_subscriber_in_topic(const Topic *topic, const char *node_name);

/**
 * @brief Node나 Topic 이름이 유효한지 검사한다.
 * 
 * simulator.c 내부에서만 사용하는 보조 함수.
 * 외부 파일에서는 호출할 수 없도록 static으로 제한한다.
 * 
 * 유효 조건:
 * 1. name 포인터가 NULL이 아니어야 한다.
 * 2. 빈 문자열("")이 아니어야 한다.
 * 3. name 길이가 SIM_NAME_LENGTH보다 작아야 한다.
 * 
 * @return 1(유효한 이름), 0(잘못된 이름)
 */
static int is_valid_name(const char *name) {
    return name != NULL && name[0] != '\0' && strlen(name) < SIM_NAME_LENGTH;
}

/**
 * @brief Node 연결 리스트 전체를 순회하며 동적 할당된 메모리를 해제한다.
 * 
 * 주의:
 * free(current)를 호출하기 전에 current->next를 next에 저장한다.
 * free 이후 current->next에 접근하면 이미 해제된 메모리에 접근하는 문제가 생긴다.
 * 
 * 시간복잡도: O(N) 
 */
static void destroy_node_list(Node *head) {
    Node *current = head;

    /* Free every node in the linked list one by one. */
    while (current != NULL) {
        Node *next = current->next; // free 전에 다음 노드 주소를 보관
        free(current); // 현재 노드 메모리 해제
        current = next; // 다음 노드로 이동
    }
}

/**
 * @brief Topic 연결 리스트와 각 Topic 내부의 Publisher/Subscriber 리스트를 해제한다.
 *
 * 각 Topic은 Publisher 연결 리스트와 Subscriber 연결 리스트를 소유한다.
 * 따라서 Topic 자체를 free하기 전에 내부 Publisher/Subscriber 리스트를 먼저 순회하며 해제해야 한다.
 *
 * 해제 순서:
 * 1. 다음 Topic 주소를 next에 저장한다.
 * 2. 현재 Topic의 Publisher 리스트를 모두 free한다.
 * 3. 현재 Topic의 Subscriber 리스트를 모두 free한다.
 * 4. 현재 Topic을 free한다.
 * 5. next로 이동한다.
 *
 * @param head Topic 연결 리스트의 head pointer
 *
 * @note 시간복잡도는 O(T + P_total + S_total)이다.
 *       T는 Topic 수, P_total은 전체 Publisher 등록 수,
 *       S_total은 전체 Subscriber 등록 수이다.
 */
static void destroy_topic_list(Topic *head) {
    Topic *current = head;

    /* Free every topic in the linked list one by one. */
    while (current != NULL) {
        Topic *next = current->next;
        Publisher *publisher = current->publishers;
        Subscriber *subscriber = current->subscribers;
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
 * @brief Simulator 구조체를 빈 상태로 초기화한다.
 * 
 * nodes와 topics는 각각 연결 리스트의 head pointer이다.
 * 초기 상태에서는 등록된 Node/Topic이 없으므로 NULL로 설정한다.
 */
void simulator_init(Simulator *sim) {
    if (sim == NULL) {
        return;
    }

    sim->nodes = NULL;
    sim->topics = NULL;
}


/**
 * @brief Simulator가 소유한 모든 연결 리스트 메모리를 해제한다.
 * 
 * destroy_node_list()와 destroy_topic_list()를 통해
 * 각 리스트의 노드를 모두 free한 뒤, head pointer를 NULL로 되돌린다.
 */
void simulator_destroy(Simulator *sim) {
    if (sim == NULL) {
        return;
    }

    destroy_node_list(sim->nodes);
    destroy_topic_list(sim->topics);
    sim->nodes = NULL;
    sim->topics = NULL;
}


/**
 * @brief Node 연결 리스트에서 name과 같은 이름의 Node를 탐색한다.
 * 
 * 탐색 방식:
 * - head부터 시작하여 next 포인터를 따라 순차 탐색한다.
 * - strcmp() 결과가 0이면 같은 문자열이므로 해당 Node를 반환한다.
 * 
 * 시간복잡도: O(N)
 */
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

/**
 * @brief Topic 연결 리스트에서 name과 같은 이름의 Topic을 탐색한다.
 * 
 * 탐색 방식:
 * - head부터 시작하여 next 포인터를 따라 순차 탐색한다.
 * - strcmp() 결과가 0이면 같은 문자열이므로 해당 Topic을 반환한다.
 * 
 * 시간복잡도: O(N)
 */
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

/**
 * @brief 특정 Topic에서 node_name에 해당하는 Publisher를 찾는다.
 * 
 * 내부적으로 find_publisher_in_topic()을 호출한다.
 * 
 * @param topic 탐색할 Topic 포인터
 * @param node_name 찾을 Publisher Node 이름
 * @return 찾은 Publisher 포인터, 없으면 NULL
 */
Publisher *simulator_find_publisher(const Topic *topic, const char *node_name) {
    return find_publisher_in_topic(topic, node_name);
}

/**
 * @brief 특정 Topic에서 node_name에 해당하는 Subscriber를 찾는다.
 * 
 * 내부적으로 find_subscriber_in_topic()을 호출한다.
 * 
 * @param topic 탐색할 Topic 포인터
 * @param node_name 찾을 Subscriber Node 이름
 * @return 찾은 Subscriber 포인터, 없으면 NULL
 */
Subscriber *simulator_find_subscriber(const Topic *topic, const char *node_name) {
    return find_subscriber_in_topic(topic, node_name);
}

/**
 * @brief Topic 내부 Publisher 연결 리스트에서 특정 Node 이름을 탐색한다.
 *
 * topic->publishers를 head pointer로 사용하여 Publisher 리스트를 순차 탐색한다.
 * strcmp() 결과가 0이면 같은 Node 이름이므로 해당 Publisher를 반환한다.
 *
 * @param topic     Publisher 목록을 가진 Topic 포인터
 * @param node_name 찾을 Publisher Node 이름
 * @return 찾은 Publisher 포인터, 없으면 NULL
 *
 * @note 시간복잡도는 Topic에 등록된 Publisher 수를 P라고 할 때 O(P)이다.
 */
static Publisher *find_publisher_in_topic(const Topic *topic, const char *node_name) {
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

/**
 * @brief Topic 내부 Subscriber 연결 리스트에서 특정 Node 이름을 탐색한다.
 *
 * topic->subscribers를 head pointer로 사용하여 Subscriber 리스트를 순차 탐색한다.
 * strcmp() 결과가 0이면 같은 Node 이름이므로 해당 Subscriber를 반환한다.
 *
 * @param topic     Subscriber 목록을 가진 Topic 포인터
 * @param node_name 찾을 Subscriber Node 이름
 * @return 찾은 Subscriber 포인터, 없으면 NULL
 *
 * @note 시간복잡도는 Topic에 등록된 Subscriber 수를 S라고 할 때 O(S)이다.
 */
static Subscriber *find_subscriber_in_topic(const Topic *topic, const char *node_name) {
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
 * @brief 특정 Topic의 Publisher 목록을 출력한다.
 *
 * topic->publishers 연결 리스트를 순회하며 각 Publisher의 node_name을 출력한다.
 * Publisher가 없는 경우에는 (empty)를 출력한다.
 *
 * @param topic Publisher 목록을 출력할 Topic 포인터
 *
 * @note 시간복잡도는 해당 Topic의 Publisher 수를 P라고 할 때 O(P)이다.
 */
static void print_publisher_list(const Topic *topic) {
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
 * @brief 특정 Topic의 Subscriber 목록을 출력한다.
 *
 * topic->subscribers 연결 리스트를 순회하며 각 Subscriber의 node_name을 출력한다.
 * Subscriber가 없는 경우에는 (empty)를 출력한다.
 *
 * @param topic Subscriber 목록을 출력할 Topic 포인터
 *
 * @note 시간복잡도는 해당 Topic의 Subscriber 수를 S라고 할 때 O(S)이다.
 */
static void print_subscriber_list(const Topic *topic) {
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
 * @brief 새로운 Node를 연결 리스트의 head에 삽입한다.
 * 
 * 처리 순서:
 * 1. Simulator와 name의 유효성을 검사한다.
 * 2. 같은 이름의 Node가 있는지 중복 검사한다.
 * 3. 새 Node 메모리를 malloc으로 할당한다.
 * 4. name을 복사한다.
 * 5. 새 Node를 리스트의 맨 앞에 삽입한다.
 * 
 * @return 1(등록 성공 시), 0(실패 시)
 * 
 * 시간 복잡도:
 * - 중복 검사: O(N)
 * - head 삽입: O(1)
 * - 전체: O(N)
 */
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


/**
 * @brief 새로운 Topic을 연결 리스트의 head에 삽입한다.
 * 
 * 처리 순서:
 * 1. Simulator와 name의 유효성을 검사한다.
 * 2. 같은 이름의 Tocpic이 있는지 중복 검사한다.
 * 3. 새 Tocpic 메모리를 malloc으로 할당한다.
 * 4. name을 복사한다.
 * 5. 새 Topic을 리스트의 맨 앞에 삽입한다.
 * 
 * @return 1(등록 성공 시), 0(실패 시)
 * 
 * 시간 복잡도:
 * - 중복 검사: O(N)
 * - head 삽입: O(1)
 * - 전체: O(N)
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

    /* Insert the new topic at the head of the linked list in O(1) time. */
    strcpy(new_topic->name, name);

    /**
     * 새 Topic은 생성 직후 Publisher/Subscriber가 없으므로
     * 각 연결 리스트의 head pointer를 NULL로 초기화한다.
     */
    new_topic->publishers = NULL;
    new_topic->subscribers = NULL;

    new_topic->next = sim->topics;
    sim->topics = new_topic;

    return 1;
}


/**
 * @brief 특정 Node를 특정 Topic의 Publisher로 등록한다.
 *
 * Node와 Topic이 모두 이미 Simulator에 등록되어 있어야 한다.
 * 같은 Topic에 같은 Node를 Publisher로 중복 등록할 수 없다.
 * 등록 시 Publisher 구조체를 동적 할당하고, Topic의 Publisher 연결 리스트 head에 삽입한다.
 *
 * 처리 순서:
 * 1. 입력 유효성 검사
 * 2. Node 존재 여부 확인
 * 3. Topic 존재 여부 확인
 * 4. Publisher 중복 등록 여부 확인
 * 5. Publisher 메모리 할당
 * 6. node_name 복사
 * 7. Topic의 Publisher 리스트 head에 삽입
 *
 * @param sim        Simulator 포인터
 * @param node_name  Publisher로 등록할 Node 이름
 * @param topic_name Publisher를 등록할 Topic 이름
 * @return 성공 시 1, 실패 시 0
 *
 * @note 시간복잡도는 O(N + T + P)이다.
 *       N은 Node 수, T는 Topic 수, P는 해당 Topic의 Publisher 수이다.
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
 * @brief 특정 Node를 특정 Topic의 Subscriber로 등록한다.
 *
 * Node와 Topic이 모두 이미 Simulator에 등록되어 있어야 한다.
 * 같은 Topic에 같은 Node를 Subscriber로 중복 등록할 수 없다.
 * 등록 시 Subscriber 구조체를 동적 할당하고, Topic의 Subscriber 연결 리스트 head에 삽입한다.
 *
 * 처리 순서:
 * 1. 입력 유효성 검사
 * 2. Node 존재 여부 확인
 * 3. Topic 존재 여부 확인
 * 4. Subscriber 중복 등록 여부 확인
 * 5. Subscriber 메모리 할당
 * 6. node_name 복사
 * 7. Topic의 Subscriber 리스트 head에 삽입
 *
 * @param sim        Simulator 포인터
 * @param node_name  Subscriber로 등록할 Node 이름
 * @param topic_name Subscriber를 등록할 Topic 이름
 * @return 성공 시 1, 실패 시 0
 *
 * @note 시간복잡도는 O(N + T + S)이다.
 *       N은 Node 수, T는 Topic 수, S는 해당 Topic의 Subscriber 수이다.
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

int simulator_publish_message(Simulator *sim, const char *node_name, const char *topic_name, const char *message, int priority)
{
    Topic *topic;

    if (sim == NULL || !is_valid_name(node_name) || !is_valid_name(topic_name) || message == NULL || message[0] == '\0') {
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

    printf("Message published:\n");
    printf("Topic: %s\n", topic_name);
    printf("Publisher: %s\n", node_name);
    printf("Data: %s\n", message);
    printf("Priority: %d\n", priority);
    return 1;
}


/**
 * @brief 등록된 Node 연결 리스트를 처음부터 끝까지 순회하면 출력한다.
 * 
 * 주의:
 * Node는 head insertion 방식으로 추가되므로,
 * 출력 순서는 등록 순서의 역순이 될 수 있다.
 * 
 * 시간복잡도: O(N)
 */
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

/**
 * @brief 등록된 Topic과 각 Topic의 Publisher/Subscriber 목록을 출력한다.
 *
 * Topic 연결 리스트를 순회하면서 Topic 이름을 출력하고,
 * 각 Topic 내부의 Publisher 리스트와 Subscriber 리스트도 함께 출력한다.
 *
 * @param sim Simulator 포인터
 *
 * @note Topic은 head insertion 방식으로 추가되므로 출력 순서는 등록 순서의 역순일 수 있다.
 * @note 시간복잡도는 O(T + P_total + S_total)이다.
 */
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
        print_publisher_list(current);
        print_subscriber_list(current);
        index++;
        current = current->next;
    }
}

/**
 * @brief 현재 시뮬레이터에 등록된 전체 목록을 출력한다.
 * 
 * 내부적으로 Node 출력 함수와 Topic 출력 함수를 재사용한다.
 */
void simulator_print_registered_lists(const Simulator *sim) {
    simulator_print_nodes(sim);
    simulator_print_topics(sim);
}
