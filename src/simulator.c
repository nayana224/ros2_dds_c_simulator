#include "simulator.h"

#include <stdio.h> // printf
#include <stdlib.h> // malloc, free
#include <string.h> // strlen, strcmp, strcpy

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
 * @brief Topic 연결 리스트 전체를 순회하며 동적 할당된 메모리를 해제한다.
 * 
 * Node 리스트 해제와 같은 방식으로,
 * free 전에 next 포인트를 먼저 저장해야 안전하다.
 */
static void destroy_topic_list(Topic *head) {
    Topic *current = head;

    /* Free every topic in the linked list one by one. */
    while (current != NULL) {
        Topic *next = current->next;
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
int simulator_add_topic(Simulator *sim, const char *name) {
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
    new_topic->next = sim->topics;
    sim->topics = new_topic;

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
 * @brief 등록된 Topic 연결 리스트를 처음부터 끝까지 순회하면 출력한다.
 * 
 * 주의:
 * Topic는 head insertion 방식으로 추가되므로,
 * 출력 순서는 등록 순서의 역순이 될 수 있다.
 * 
 * 시간복잡도: O(N)
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
        index++;
        current = current->next;
    }
}

/**
 * @brief 현재 시뮬레이터에 등록된 전체 목록을 출력하낟.
 * 
 * 내부적으로 Node 출력 함수와 Topic 출력 함수를 재사용한다.
 */
void simulator_print_registered_lists(const Simulator *sim) {
    simulator_print_nodes(sim);
    simulator_print_topics(sim);
}
