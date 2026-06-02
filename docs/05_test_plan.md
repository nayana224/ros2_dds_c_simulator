# 05. 테스트 계획

## Test-01. Node 등록 테스트
입력:
- add_node lidar_node

기대 결과:
- Node list에 lidar_node가 추가된다.

## Test-02. Topic 등록 테스트
입력:
- add_topic /scan

기대 결과:
- Topic list에 /scan이 추가된다.

## Test-03. Publisher/Subscriber 등록 테스트
입력:
- add_publisher lidar_node /scan
- add_subscriber nav_node /scan

기대 결과:
- /scan Topic의 publisher list에 lidar_node가 존재한다.
- /scan Topic의 subscriber list에 nav_node가 존재한다.

## Test-04. FIFO Message Queue 테스트
입력:
- publish msg1 priority 1
- publish msg2 priority 1
- receive

기대 결과:
- msg1이 먼저 수신된다.

## Test-05. Priority Queue 테스트
입력:
- publish normal_msg priority 1
- publish emergency_stop priority 10
- receive

기대 결과:
- emergency_stop이 먼저 수신된다.

## Test-06. Graph BFS 테스트
입력:
- lidar_node -> /scan -> nav_node
- nav_node -> /cmd_vel -> motor_node
- search lidar_node motor_node

기대 결과:
- 통신 경로가 존재한다고 출력된다.