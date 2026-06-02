# AGENTS.md

## Project Goal
Implement a C-based simulator that mimics ROS2-style Pub/Sub communication using data structures.

## Important Constraints
- Use C language only.
- Do not use actual ROS2 libraries.
- Do not use DDS, RTPS, sockets, or external networking libraries.
- The simulator must run locally on a single laptop.
- Focus on data structures: linked list, queue, priority queue, graph.
- Keep the code simple enough for a data structure course final project.
- Every major function must have comments explaining the data structure operation.

## Build
Use Makefile.

Commands:
- `make`
- `make run`
- `make test`
- `make clean`

## Implementation Rule
Before modifying code:
1. Read docs/01_requirements.md.
2. Read docs/02_data_structure_design.md.
3. Read docs/05_test_plan.md.
4. Implement only the requested task.
5. Do not rewrite unrelated files.

## Testing Rule
After implementation:
1. Run `make test`.
2. Run at least one example scenario.
3. Report what passed and what failed.