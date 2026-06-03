#include "cli.h"

int main(void) {
    Simulator sim;

    simulator_init(&sim);
    cli_run(&sim);
    simulator_destroy(&sim);

    return 0;
}
