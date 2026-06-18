# 00. Project Overview

## 1. Problem statement

ROS2 systems exchange messages through nodes and topics, typically using DDS
middleware. This project does not use real ROS2 or DDS. Instead, it models the
same high-level pub/sub concepts with basic C data structures.

## 2. Approach

The simulator represents:

- nodes and topics with linked structures
- publisher/subscriber relationships as topic-attached lists
- queued messages with a per-topic priority queue
- communication reachability with graph-style traversal and BFS

## 3. Expected outcome

Users can:

- register nodes and topics
- connect publishers and subscribers
- publish and receive messages
- inspect communication relationships
- search for message paths between nodes

## 4. Scope

- no real ROS2 library usage
- no real network communication
- simplified modeling of pub/sub, topic matching, QoS-like priority, and path search
