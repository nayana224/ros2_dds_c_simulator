# C 기반 ROS2 Pub/Sub 내부 통신 시뮬레이터

## 프로젝트 목적
본 프로젝트는 ROS2의 DDS 기반 Pub/Sub 통신 구조를 C언어 자료구조로 단순화하여 구현한다.

## 핵심 기능
- Node 등록
- Topic 등록
- Publisher / Subscriber 등록
- Message publish / receive
- Topic별 Message Queue 관리
- QoS priority 기반 우선순위 메시지 처리
- Node 간 통신 그래프 출력
- BFS 기반 통신 가능 여부 탐색

## 사용 자료구조
- 연결 리스트
- 큐
- 우선순위 큐
- 그래프