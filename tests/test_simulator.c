#include "simulator.h"

#include <stdio.h>

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
