/**
 * @file test_simulator.c
 * @brief simulator.c가 제대로 작동하는지 자동으로 확인하는 테스트 코드.
 * 
 * 이 파일은 Node와 Topic을 연결 리스트로 관리하기 위한
 * 구조체와 함수 인터페이스를 정의한다.
 */

#include "simulator.h"

#include <stdio.h>

/**
 * @brief 테스트 조건을 검사하는 내부 보조 함수.
 * 
 * condition이 참이면 PASS 출력하고 1을 반환한다.
 * condition이 거짓이면 FAIL 출력하고 0을 반환한다.
 */
static int assert_true(int condition, const char *message) {
    if (!condition) {
        printf("FAIL: %s\n", message);
        return 0;
    }

    printf("PASS: %s\n", message);
    return 1;
}

int main(void) {
    Simulator sim;
    int ok = 1;

    simulator_init(&sim);

    ok &= assert_true(simulator_add_node(&sim, "lidar_node") == 1,
        "FR-01 node registration succeeds");
    ok &= assert_true(simulator_find_node(&sim, "lidar_node") != NULL,
        "registered node can be found");
    ok &= assert_true(simulator_add_node(&sim, "lidar_node") == 0,
        "duplicate node registration is rejected");

    ok &= assert_true(simulator_add_topic(&sim, "/scan") == 1,
        "FR-02 topic registration succeeds");
    ok &= assert_true(simulator_find_topic(&sim, "/scan") != NULL,
        "registered topic can be found");
    ok &= assert_true(simulator_add_topic(&sim, "/scan") == 0,
        "duplicate topic registration is rejected");

    simulator_destroy(&sim);

    if (!ok) {
        return 1;
    }

    printf("All requested FR-01/FR-02 tests passed.\n");
    return 0;
}
