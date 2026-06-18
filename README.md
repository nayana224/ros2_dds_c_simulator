# ROS2 DDS C Simulator

This project is a lightweight C simulator that models ROS2-style pub/sub
communication with basic data structures instead of real ROS2 or DDS
middleware.

It is designed as a data structure course project, so the implementation keeps
the concepts visible and the code relatively simple.

## What the project demonstrates

- node registration
- topic registration
- publisher/subscriber relationships
- message publish and receive
- per-topic priority queue behavior
- communication graph output
- BFS-based path search between nodes

## Constraints

- C language only
- no ROS2 libraries
- no DDS, RTPS, sockets, or external networking
- single-machine local execution only
- focus on linked list, queue, priority queue, and graph concepts

## Current project structure

```text
include/
  cli.h
  simulator.h
  simulator_internal.h

src/
  main.c
  cli.c
  simulator_lifecycle.c
  simulator_registry.c
  simulator_message.c
  simulator_graph.c

tests/
  test_simulator.c

examples/
  fr01_fr02_demo.txt
  fr03_demo.txt
  fr03_fr04_demo.txt
  fr05_demo.txt

docs/
  00_project_overview.md
  01_requirements.md
  02_data_structure_design.md
  03_architecture.md
  04_interface_spec.md
  05_test_plan.md
  06_complexity_analysis.md
  07_final_prompt.md
  08_architecture_notes.md
```

## Responsibility split

- `main.c`
  - program entry point
- `cli.c`
  - command parsing and user-facing output
- `simulator_lifecycle.c`
  - simulator initialization and cleanup
- `simulator_registry.c`
  - node/topic/pub/sub registration and lookup
- `simulator_message.c`
  - message creation, queue ordering, publish/receive
- `simulator_graph.c`
  - list/graph/path formatting and BFS path search

## Data structure design diagram

```text
Simulator
|
+-- nodes ------------------------------------> singly linked list of Node
|                                                [Node] -> [Node] -> ...
|
+-- topics -----------------------------------> singly linked list of Topic
                                                 [Topic] -> [Topic] -> ...
                                                    |
                                                    +-- publishers -------> singly linked list of Publisher
                                                    |                        [Publisher] -> ...
                                                    |
                                                    +-- subscribers ------> singly linked list of Subscriber
                                                    |                        [Subscriber] -> ...
                                                    |
                                                    +-- message_head -----> priority-ordered Message queue
                                                    |                        [Message] -> [Message] -> ...
                                                    |
                                                    +-- message_tail -----> rear pointer of the same queue
```

Each `Topic` acts as a hub that owns three internal structures:
publisher list, subscriber list, and message queue. This is why the project
works well as a nested data structure example rather than a single flat list.

## Build and run

On Windows PowerShell with MinGW:

```powershell
mingw32-make
mingw32-make run
```

Run tests:

```powershell
mingw32-make test
```

Clean build outputs:

```powershell
mingw32-make clean
```

## CLI commands

```text
add_node <node>
add_topic <topic>
add_publisher <node> <topic>
add_subscriber <node> <topic>
publish <publisher_node> <topic> <message> <priority>
receive <subscriber_node> <topic>
list
graph
search <start_node> <target_node>
help
exit
```

Use quotes for messages with spaces:

```text
publish lidar_node /scan "range data" 5
```

## Example scenario

```text
add_node lidar_node
add_node nav_node
add_node motor_node
add_topic /scan
add_topic /cmd_vel
add_publisher lidar_node /scan
add_subscriber nav_node /scan
add_publisher nav_node /cmd_vel
add_subscriber motor_node /cmd_vel
publish lidar_node /scan "normal data" 1
publish lidar_node /scan "emergency data" 10
receive nav_node /scan
graph
search lidar_node motor_node
exit
```

Expected key output:

```text
Message received:
Topic: /scan
Subscriber: nav_node
Data: emergency data
Priority: 10

Communication graph:
  lidar_node -> /scan
  /scan -> nav_node
  nav_node -> /cmd_vel
  /cmd_vel -> motor_node

Path search:
  Path found: lidar_node -> /scan -> nav_node -> /cmd_vel -> motor_node
```

Expected in-memory structure after the example scenario:

```text
Simulator sim
|
+-- nodes
|     |
|     v
|   [motor_node] -> [nav_node] -> [lidar_node] -> NULL
|
+-- topics
      |
      v
    [/cmd_vel] -> [/scan] -> NULL
       |            |
       |            +-- publishers
       |            |     |
       |            |     v
       |            |   [node_name: lidar_node] -> NULL
       |            |
       |            +-- subscribers
       |            |     |
       |            |     v
       |            |   [node_name: nav_node] -> NULL
       |            |
       |            +-- messages
       |                  |
       |                  v
       |               [normal data | priority 1] -> NULL
       |
       +-- publishers
       |     |
       |     v
       |   [node_name: nav_node] -> NULL
       |
       +-- subscribers
       |     |
       |     v
       |   [node_name: motor_node] -> NULL
       |
       +-- messages
             |
             v
           NULL
```

The `/scan` queue contains only `normal data` at this point because
`emergency data` was already dequeued by the `receive nav_node /scan` command.

## Notes

- This simulator models concepts, not real ROS2 runtime behavior.
- All state is stored in local process memory only.
- Registered objects and queued messages are lost when the program exits.
