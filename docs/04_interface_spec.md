# 04. Interface Specification

## 1. CLI command interface

The program provides a text-based command-line interface.
After startup, the user enters one command per line.

Supported commands:

1. `add_node <node>`
2. `add_topic <topic>`
3. `add_publisher <node> <topic>`
4. `add_subscriber <node> <topic>`
5. `publish <publisher_node> <topic> <message> <priority>`
6. `receive <subscriber_node> <topic>`
7. `list`
8. `graph`
9. `search <start_node> <target_node>`
10. `help`
11. `exit`

Messages containing spaces must be wrapped in double quotes.

Example:

```text
publish lidar_node /scan "range data" 5
```

## 2. Input examples

### Add node

```text
add_node lidar_node
```

### Add topic

```text
add_topic /scan
```

### Add publisher

```text
add_publisher lidar_node /scan
```

### Add subscriber

```text
add_subscriber nav_node /scan
```

### Publish message

```text
publish lidar_node /scan "range data" 5
```

### Receive message

```text
receive nav_node /scan
```

### Print registered lists

```text
list
```

### Print communication graph

```text
graph
```

### Search path between nodes

```text
search lidar_node motor_node
```

## 3. Output examples

### Publish result

```text
Message published:
Topic: /scan
Publisher: lidar_node
Data: range data
Priority: 5
```

### Receive result

```text
Message received:
Topic: /scan
Subscriber: nav_node
Data: range data
Priority: 5
```

### Graph result

```text
Communication graph:
  lidar_node -> /scan
  /scan -> nav_node
```

### Path search result

```text
Path search:
  Path found: lidar_node -> /scan -> nav_node -> /cmd_vel -> motor_node
```

## 4. Interface responsibility note

The CLI is responsible for:

- reading commands
- parsing arguments
- validating command shape
- printing final user-facing output

The simulator core is responsible for:

- updating simulator state
- managing data structures
- formatting query results for the CLI
