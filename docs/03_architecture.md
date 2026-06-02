# 03. 시스템 아키텍처

## 1. 전체 구조

User Command
    ↓
Simulator Core
    ↓
Node Manager / Topic Manager / Message Queue / Communication Graph

## 2. 데이터 흐름

1. 사용자가 Node를 등록한다.
2. 사용자가 Topic을 등록한다.
3. Node를 Publisher 또는 Subscriber로 연결한다.
4. Publisher가 Topic에 Message를 발행한다.
5. Topic은 Message를 Queue에 저장한다.
6. Subscriber는 Topic Queue에서 Message를 수신한다.
7. Graph는 Node 간 통신 관계를 저장하고 탐색한다.

## 3. ROS2 개념과의 대응

| ROS2 개념 | 본 프로젝트의 단순화 구현 |
|---|---|
| Node | Node 구조체 |
| Topic | Topic 구조체 |
| Publisher | Topic 내부 Publisher List |
| Subscriber | Topic 내부 Subscriber List |
| DDS Message Queue | Topic 내부 MessageQueue |
| QoS Priority | priority 기반 Message 정렬 |
| ROS Graph | Graph adjacency list |