# ROS2 DDS C Simulator

ROS2의 Pub/Sub와 DDS 메시지 전달 개념을 실제 ROS2 라이브러리 없이 C언어 자료구조로 단순화한 시뮬레이터입니다.

이 프로젝트는 자료구조 수업 최종 프로젝트를 목표로 하며, Node, Topic, Publisher, Subscriber, Message Queue, Priority Queue, Communication Graph, BFS Path Search를 직접 구현합니다.

## 주요 특징

- 실제 ROS2, DDS, RTPS, socket, 외부 네트워크 라이브러리를 사용하지 않습니다.
- 모든 통신 관계는 하나의 로컬 C 프로그램 안에서 시뮬레이션합니다.
- Node, Topic, Publisher, Subscriber는 연결 리스트로 관리합니다.
- Message는 Topic 내부 priority queue에 저장합니다.
- priority가 높은 Message가 먼저 수신됩니다.
- priority가 같은 Message는 발행 순서를 유지합니다.
- Communication Graph를 출력할 수 있습니다.
- Node 간 메시지 전달 가능 경로를 BFS로 탐색할 수 있습니다.
- CLI는 ROS2 명령어 스타일을 참고한 명령어 기반 인터페이스입니다.

## 프로젝트 구조

```text
include/
  cli.h
  simulator.h

src/
  main.c
  cli.c
  simulator.c

tests/
  test_simulator.c

docs/
  00_project_overview.md
  01_requirements.md
  02_data_structure_design.md
  03_architecture.md
  04_interface_spec.md
  05_test_plan.md
  06_complexity_analysis.md
  07_final_prompt.md
```

## 빌드 및 실행

Windows PowerShell에서 MinGW Make를 사용하는 경우:

```powershell
mingw32-make
mingw32-make run
```

테스트 실행:

```powershell
mingw32-make test
```

빌드 결과물 삭제:

```powershell
mingw32-make clean
```

## CLI 명령어

프로그램 실행 후 다음 명령어를 입력할 수 있습니다.

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

메시지에 공백이 있는 경우 따옴표를 사용합니다.

```text
publish lidar_node /scan "range data" 5
```

## 예시 시나리오

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

예상되는 주요 출력:

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

## 구현된 요구사항

| ID | 기능 | 구현 상태 |
|---|---|---|
| FR-01 | Node 등록 | 완료 |
| FR-02 | Topic 등록 | 완료 |
| FR-03 | Publisher 등록 | 완료 |
| FR-04 | Subscriber 등록 | 완료 |
| FR-05 | Message Publish | 완료 |
| FR-06 | Message Queue | 완료 |
| FR-07 | Message Receive | 완료 |
| FR-08 | QoS Priority | 완료 |
| FR-09 | Communication Graph | 완료 |
| FR-10 | Path Search | 완료 |

## 사용 자료구조

| 자료구조 | 사용 위치 |
|---|---|
| 연결 리스트 | Node, Topic, Publisher, Subscriber 관리 |
| Priority Queue | Topic별 Message 저장 및 수신 순서 관리 |
| Queue | BFS 탐색 중 방문할 Node 관리 |
| Graph | Publisher -> Topic -> Subscriber 통신 관계 표현 |

## 제한 사항

- 실제 ROS2 Node나 DDS middleware와 통신하지 않습니다.
- 여러 프로세스 간 통신을 수행하지 않습니다.
- 모든 상태는 프로그램 실행 중 메모리에만 저장됩니다.
- 프로그램 종료 시 등록 정보와 메시지는 사라집니다.
