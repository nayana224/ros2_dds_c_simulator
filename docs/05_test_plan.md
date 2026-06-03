# 05. 테스트 계획

## 1. 테스트 목표

이 문서는 ROS2 DDS C Simulator의 기능 요구사항 FR-01부터 FR-10까지를 검증하기 위한 테스트 시나리오를 정리한다.

테스트는 두 가지 방식으로 수행한다.

- 자동 테스트: `mingw32-make test`
- 수동 CLI 테스트: `mingw32-make run` 실행 후 명령어 입력

## 2. 자동 테스트

### 실행 명령

```powershell
mingw32-make test
```

### 검증 범위

| ID | 검증 기능 | 주요 확인 내용 |
|---|---|---|
| Test-01 | Node 등록 | 정상 등록, 중복 등록 거부 |
| Test-02 | Topic 등록 | 정상 등록, Topic 검색 |
| Test-03 | Publisher 등록 | Node/Topic 존재 확인, 중복 등록 거부 |
| Test-04 | Subscriber 등록 | Node/Topic 존재 확인, 중복 등록 거부 |
| Test-05 | Message Publish | Publisher만 메시지를 발행할 수 있음 |
| Test-06 | Message Receive | Subscriber만 메시지를 수신할 수 있음 |
| Test-07 | Priority Queue | 높은 priority 메시지가 먼저 수신됨 |
| Test-08 | Same Priority Order | 같은 priority 메시지는 발행 순서를 유지함 |
| Test-09 | Communication Graph | publish/subscribe 관계 출력 함수 호출 |
| Test-10 | BFS Path Search | Node 간 메시지 전달 경로 탐색 성공/실패 |

## 3. 수동 CLI 테스트 시나리오

### Scenario-01. 기본 등록 및 목록 출력

입력:

```text
add_node lidar_node
add_node nav_node
add_topic /scan
add_publisher lidar_node /scan
add_subscriber nav_node /scan
list
exit
```

기대 결과:

- `lidar_node`, `nav_node`가 Node 목록에 출력된다.
- `/scan` Topic 아래 Publisher `lidar_node`가 출력된다.
- `/scan` Topic 아래 Subscriber `nav_node`가 출력된다.

### Scenario-02. Priority Queue 기반 publish/receive

입력:

```text
add_node lidar_node
add_node nav_node
add_topic /scan
add_publisher lidar_node /scan
add_subscriber nav_node /scan
publish lidar_node /scan "normal data" 1
publish lidar_node /scan "emergency data" 10
receive nav_node /scan
receive nav_node /scan
exit
```

기대 결과:

- 첫 번째 수신 메시지는 `emergency data`, priority `10`이다.
- 두 번째 수신 메시지는 `normal data`, priority `1`이다.

### Scenario-03. Communication Graph 출력

입력:

```text
add_node lidar_node
add_node nav_node
add_topic /scan
add_publisher lidar_node /scan
add_subscriber nav_node /scan
graph
exit
```

기대 결과:

```text
Communication graph:
  lidar_node -> /scan
  /scan -> nav_node
```

### Scenario-04. BFS 경로 탐색

입력:

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
search lidar_node motor_node
search motor_node lidar_node
exit
```

기대 결과:

```text
Path search:
  Path found: lidar_node -> /scan -> nav_node -> /cmd_vel -> motor_node
```

두 번째 탐색은 방향 그래프상 역방향 경로가 없으므로 다음과 같이 출력된다.

```text
Path search:
  Path not found.
```

## 4. 예외 테스트

| 입력 예시 | 기대 결과 |
|---|---|
| `add_node lidar_node`를 두 번 실행 | 두 번째 등록 실패 |
| 존재하지 않는 Node로 `add_publisher` 실행 | 등록 실패 |
| Subscriber가 아닌 Node로 `receive` 실행 | 수신 실패 |
| Publisher가 아닌 Node로 `publish` 실행 | 발행 실패 |
| 빈 Queue에서 `receive` 실행 | 수신 실패 |
| 잘못된 명령어 입력 | `Unknown command` 또는 사용법 오류 출력 |
