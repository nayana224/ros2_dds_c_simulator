/**
 * @file main.c
 * @brief ROS2 Pub/Sub C Simulator의 메뉴 기반 CLI 진입점.
 * 
 * 이 파일은 사용자의 메뉴 입력을 받아 Node 등록, Topic 등록,
 * 등록 목록 출력 기능을 실행한다.
 */

#include "simulator.h"

#include <stdio.h>
#include <string.h>


/**
 * @brief 표준 입력에서 이름 문자열을 읽고 개행 문자를 제거한다.
 * 
 * fgets()를 사용하여 버퍼 크기를 넘지 않도록 입력을 제한한다.
 * 입력 끝에 포함될 수 있는 '\n' 또는 '\r\n'은 제거하여
 * Node/Topic 이름 비교가 정확히 동작하도록 한다.
 * 
 * @param buffer 입력 문자열을 저장할 버퍼
 * @param size   buffer의 전체 크기
 */
static void read_name_input(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';
}

/**
 * @brief 사용자가 선택할 수 있는 메뉴를 출력한다. 
 */
static void print_menu(void) {
    printf("\n=== ROS2 Pub/Sub C Simulator ===\n");
    printf("1. Add Node\n");
    printf("2. Add Topic\n");
    printf("3. Add Publisher (not implemented)\n");
    printf("4. Add Subscriber (not implemented)\n");
    printf("5. Publish Message (not implemented)\n");
    printf("6. Receive Message (not implemented)\n");
    printf("7. Print Registered Lists\n");
    printf("8. Print Communication Graph (not implemented)\n");
    printf("9. Search Path Between Nodes (not implemented)\n");
    printf("0. Exit\n");
    printf("Select: ");
}


/**
 * @brief Node 등록 메뉴 동작을 처리한다.
 * 
 * 사용자로부터 Node 이름을 입력받고 simulator_add_node()를 호출한다.
 * 등록 성공 시 성공 메시지를 출력하고, 실패 시 가능한 실패 원인을 안내한다.
 * 
 * 실패 가능한 경우:
 * - 빈 문자열 입력
 * - 이름 길이가 SIM_NAME_LENGTH 이상
 * - 이미 같은 이름의 Node가 등록됨
 * 
 * @param sim Node를 등록할 Simulator 포인터
 */
static void handle_add_node(Simulator *sim) {
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
 * @brief Topic 등록 메뉴 동작을 처리한다.
 *
 * 사용자로부터 Topic 이름을 입력받고 simulator_add_topic()을 호출한다.
 * 등록 성공 시 성공 메시지를 출력하고, 실패 시 가능한 실패 원인을 안내한다.
 *
 * 실패 가능한 경우:
 * - 빈 문자열 입력
 * - 이름 길이가 SIM_NAME_LENGTH 이상
 * - 이미 같은 이름의 Topic이 등록됨
 *
 * @param sim Topic을 등록할 Simulator 포인터
 */
static void handle_add_topic(Simulator *sim) {
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
 * @brief 프로그램의 시작점.
 *
 * Simulator를 초기화한 뒤 메뉴 기반 반복 루프를 실행한다.
 * 사용자의 메뉴 선택에 따라 Node 등록, Topic 등록, 등록 목록 출력 기능을 수행한다.
 * 프로그램 종료 전에는 simulator_destroy()를 호출하여 동적 할당된 메모리를 해제한다.
 *
 * @return 정상 종료 시 0
 */
int main(void) {
    Simulator sim;
    int choice;

    simulator_init(&sim);

    for (;;) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        /*
         * scanf("%d", ...) 이후 입력 버퍼에 남아 있는 개행 문자 '\n'을 제거한다.
         * 이를 제거하지 않으면 이후 fgets()가 빈 문자열을 읽을 수 있다.
         */
        getchar();

        switch (choice) {
            case 1:
                handle_add_node(&sim);
                break;
            case 2:
                handle_add_topic(&sim);
                break;
            case 7:
                simulator_print_registered_lists(&sim);
                break;
            case 0:
                simulator_destroy(&sim);
                return 0;
            case 3:
            case 4:
            case 5:
            case 6:
            case 8:
            case 9:
                printf("This feature is intentionally not implemented in this step.\n");
                break;
            default:
                printf("Unknown menu option.\n");
                break;
        }
    }

    simulator_destroy(&sim);
    return 0;
}
