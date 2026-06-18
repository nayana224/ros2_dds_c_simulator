# 03. Architecture

## 1. Overall structure

```text
User Command
    ->
CLI Layer
    ->
Simulator Core
    ->
Registry / Message Queue / Graph Logic
```

## 2. Layer responsibilities

### CLI layer

Handled by `src/cli.c`.

Responsibilities:

- read commands from standard input
- tokenize and validate command arguments
- call simulator APIs
- print user-facing success, failure, and query output

### Simulator core

Handled by the simulator source files.

Responsibilities:

- manage simulator state
- apply registration rules
- maintain message queue ordering
- compute graph relationships
- search reachable paths between nodes

## 3. File-level responsibility split

### `src/main.c`

- program entry point
- initializes the simulator
- runs the CLI
- destroys simulator state before exit

### `src/simulator_lifecycle.c`

- simulator initialization
- simulator destruction
- nested memory cleanup

### `src/simulator_registry.c`

- node registration and lookup
- topic registration and lookup
- publisher/subscriber registration
- shared name validation

### `src/simulator_message.c`

- message allocation
- priority queue insertion
- dequeue behavior
- publish/receive logic

### `src/simulator_graph.c`

- node/topic list formatting
- communication graph formatting
- BFS path search

### `include/simulator_internal.h`

- internal helper declarations shared across simulator implementation files

## 4. Data flow

1. The user enters a command in the CLI.
2. The CLI parses and validates the command shape.
3. The CLI calls the corresponding simulator API.
4. The simulator updates internal data structures or builds query results.
5. The CLI prints the final user-facing output.

## 5. Mapping to ROS2 concepts

| ROS2 concept | This project |
|---|---|
| Node | `Node` structure |
| Topic | `Topic` structure |
| Publisher | Topic-attached publisher list |
| Subscriber | Topic-attached subscriber list |
| DDS message queue | Per-topic message queue |
| QoS priority | Priority-based queue ordering |
| ROS graph | Publisher/topic/subscriber relationship graph |

## 6. Current architectural note

The project now has a clearer responsibility split than the original
single-file simulator core. Recent refactoring also moved the `list`,
`graph`, and `search` command flow closer to a cleaner boundary:
the simulator core builds formatted results, and the CLI is responsible for
printing them to the user.
