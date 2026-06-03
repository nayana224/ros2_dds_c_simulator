/**
 * @file main.c
 * @brief ROS2 Pub/Sub C Simulator의 프로그램 진입점.
 */

#include "cli.h"

/**
 * @brief Simulator를 초기화하고 CLI를 실행한 뒤 모든 메모리를 해제한다.
 *
 * @return 프로그램 정상 종료 시 0
 */
int main(void) {
    Simulator sim;

    simulator_init(&sim);
    cli_run(&sim);
    simulator_destroy(&sim);

    return 0;
}
