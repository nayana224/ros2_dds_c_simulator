/**
 * @file main.c
 * @brief Program entry point for the ROS2-style pub/sub simulator.
 */

#include "cli.h"

/**
 * @brief Initializes the simulator, runs the CLI, and releases resources.
 *
 * @return 0 on normal termination.
 */
int main(void) {
    Simulator sim;

    simulator_init(&sim);
    cli_run(&sim);
    simulator_destroy(&sim);

    return 0;
}
