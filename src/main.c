#include "simulator.h"

#include <stdio.h>
#include <string.h>

static void read_name_input(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';
}

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
