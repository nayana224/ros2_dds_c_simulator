/**
 * @file cli.h
 * @brief Command-line interface for the ROS2-style pub/sub simulator.
 */

#ifndef CLI_H
#define CLI_H

#include "simulator.h"

/**
 * @brief Runs the interactive command-line loop.
 *
 * This function reads user commands from standard input, dispatches them to
 * the simulator API, and prints results to standard output. The loop
 * continues until the user enters an exit command or input ends.
 *
 * @param sim Simulator instance used by the CLI.
 */
void cli_run(Simulator *sim);

#endif
