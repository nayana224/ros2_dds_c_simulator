#include "simulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    Message *message;
    Topic *topic;
    int ok = 1;

    simulator_init(&sim);

    ok &= assert_true(simulator_add_node(&sim, "lidar_node") == 1,
        "FR-01 node registration succeeds");
    ok &= assert_true(simulator_add_topic(&sim, "/scan") == 1,
        "FR-02 topic registration succeeds");

    ok &= assert_true(simulator_add_publisher(&sim, "lidar_node", "/scan") == 1,
        "FR-03 publisher registration succeeds");

    topic = simulator_find_topic(&sim, "/scan");
    ok &= assert_true(topic != NULL, "topic can be found for publisher check");
    ok &= assert_true(simulator_find_publisher(topic, "lidar_node") != NULL,
        "registered publisher can be found");
    ok &= assert_true(simulator_add_publisher(&sim, "lidar_node", "/scan") == 0,
        "duplicate publisher registration is rejected");
    ok &= assert_true(simulator_add_publisher(&sim, "nav_node", "/scan") == 0,
        "publisher registration fails when node does not exist");
    ok &= assert_true(simulator_add_publisher(&sim, "lidar_node", "/missing") == 0,
        "publisher registration fails when topic does not exist");

    ok &= assert_true(simulator_add_node(&sim, "nav_node") == 1,
        "second node registration succeeds");
    ok &= assert_true(simulator_add_subscriber(&sim, "nav_node", "/scan") == 1,
        "FR-04 subscriber registration succeeds");
    ok &= assert_true(simulator_find_subscriber(topic, "nav_node") != NULL,
        "registered subscriber can be found");
    ok &= assert_true(simulator_add_subscriber(&sim, "nav_node", "/scan") == 0,
        "duplicate subscriber registration is rejected");
    ok &= assert_true(simulator_add_subscriber(&sim, "ghost_node", "/scan") == 0,
        "subscriber registration fails when node does not exist");
    ok &= assert_true(simulator_add_subscriber(&sim, "nav_node", "/missing") == 0,
        "subscriber registration fails when topic does not exist");

    ok &= assert_true(simulator_publish_message(&sim, "lidar_node", "/scan", "range data", 5) == 1,
        "FR-05 message publish succeeds");
    ok &= assert_true(simulator_publish_message(&sim, "lidar_node", "/scan", "second data", 3) == 1,
        "second message publish succeeds");

    message = simulator_receive_message(&sim, "nav_node", "/scan");
    ok &= assert_true(message != NULL, "first queued message exists");
    ok &= assert_true(strcmp(message->data, "range data") == 0,
        "FR-07 subscriber receives first queued message in FIFO order");
    free(message);

    message = simulator_dequeue_message(topic);
    ok &= assert_true(message != NULL, "second queued message exists");
    ok &= assert_true(strcmp(message->data, "second data") == 0,
        "second queued message keeps FIFO order");
    free(message);

    ok &= assert_true(simulator_dequeue_message(topic) == NULL,
        "queue is empty after dequeuing all messages");
    ok &= assert_true(simulator_receive_message(&sim, "nav_node", "/scan") == NULL,
        "message receive fails when queue is empty");

    ok &= assert_true(simulator_publish_message(&sim, "lidar_node", "/scan", "third data", 1) == 1,
        "third message publish succeeds");
    ok &= assert_true(simulator_receive_message(&sim, "lidar_node", "/scan") == NULL,
        "message receive fails when node is not a subscriber");
    message = simulator_receive_message(&sim, "nav_node", "/scan");
    ok &= assert_true(message != NULL, "subscriber can still receive after failed receiver attempt");
    ok &= assert_true(strcmp(message->data, "third data") == 0,
        "failed receiver attempt does not dequeue message");
    free(message);

    ok &= assert_true(simulator_publish_message(&sim, "nav_node", "/scan", "range data", 5) == 0,
        "message publish fails when node is not a publisher");
    ok &= assert_true(simulator_publish_message(&sim, "lidar_node", "/missing", "range data", 5) == 0,
        "message publish fails when topic does not exist");
    ok &= assert_true(simulator_publish_message(&sim, "lidar_node", "/scan", "", 5) == 0,
        "message publish fails when message is empty");
    ok &= assert_true(simulator_dequeue_message(topic) == NULL,
        "failed message publishes do not enqueue messages");

    simulator_destroy(&sim);

    if (!ok) {
        return 1;
    }

    printf("All requested FR-01/FR-02/FR-03/FR-04/FR-05/FR-06/FR-07 tests passed.\n");
    return 0;
}
