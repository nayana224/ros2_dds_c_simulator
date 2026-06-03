# 07. 최종 프롬프트 정리

## 1. 프로젝트 구현 프롬프트

실제 ROS2 라이브러리나 하드웨어는 사용하지 않고, ROS2의 Pub/Sub와 DDS 메시지 전달 개념을 C언어 자료구조로 단순화하여 구현한다.

Node, Topic, Publisher, Subscriber는 연결 리스트로 관리하고, Message는 Topic 내부 priority queue로 처리한다. 전체 publish/subscribe 관계는 그래프 형태로 출력하며, 특정 Node에서 다른 Node까지 메시지 전달 경로가 존재하는지 BFS로 탐색한다.

프로그램은 하나의 로컬 실행 파일로 동작하며, socket, DDS, RTPS, 외부 네트워크 라이브러리는 사용하지 않는다. CLI는 명령어 기반으로 구성하여 ROS2 명령어를 사용하는 느낌을 단순하게 모방한다.

## 2. 구현 요구사항

| ID | 요구사항 | 구현 내용 |
|---|---|---|
| FR-01 | Node 등록 | `add_node <node>` 명령어로 Node를 연결 리스트에 등록 |
| FR-02 | Topic 등록 | `add_topic <topic>` 명령어로 Topic을 연결 리스트에 등록 |
| FR-03 | Publisher 등록 | `add_publisher <node> <topic>` 명령어로 Publisher 관계 등록 |
| FR-04 | Subscriber 등록 | `add_subscriber <node> <topic>` 명령어로 Subscriber 관계 등록 |
| FR-05 | Message Publish | Publisher Node가 Topic에 Message를 발행 |
| FR-06 | Message Queue | Topic 내부 Message Queue에 Message 저장 |
| FR-07 | Message Receive | Subscriber Node가 Topic에서 Message 수신 |
| FR-08 | QoS Priority | priority가 높은 Message를 먼저 수신 |
| FR-09 | Communication Graph | publish/subscribe 관계를 방향 그래프 형태로 출력 |
| FR-10 | Path Search | Node 간 전달 가능 경로를 BFS로 탐색 |

## 3. 명령어 기반 CLI 예시

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
publish lidar_node /scan "emergency data" 10
receive nav_node /scan
graph
search lidar_node motor_node
exit
```

## 4. 핵심 설계 기준

- C언어만 사용한다.
- 실제 ROS2, DDS, RTPS, socket, 외부 네트워크 라이브러리를 사용하지 않는다.
- 자료구조 수업 프로젝트 수준에 맞게 단순하고 설명 가능한 구조로 구현한다.
- 동적 할당한 메모리는 `simulator_destroy()` 또는 호출자가 `free()`로 해제한다.
- 각 주요 기능은 연결 리스트, priority queue, graph, BFS 중 어떤 자료구조를 사용하는지 설명 가능해야 한다.

## 5. 최종 검증 명령

```powershell
mingw32-make test
mingw32-make
```

수동 실행:

```powershell
mingw32-make run
```
