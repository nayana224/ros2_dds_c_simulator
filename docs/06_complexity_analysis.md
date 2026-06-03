# 06. 복잡도 분석

## 1. 기호 정의

| 기호 | 의미 |
|---|---|
| N | 등록된 Node 수 |
| T | 등록된 Topic 수 |
| P | 특정 Topic에 등록된 Publisher 수 |
| S | 특정 Topic에 등록된 Subscriber 수 |
| M | 특정 Topic에 저장된 Message 수 |
| V | 그래프 탐색에서의 정점 수, 현재 구현에서는 Node 수 |
| E | 그래프 탐색에서의 간선 수, Publisher -> Topic -> Subscriber 관계 수 |

## 2. 기능별 복잡도

| 기능 | 사용 자료구조 | 시간복잡도 | 공간복잡도 | 설명 |
|---|---|---:|---:|---|
| Node 등록 | 단일 연결 리스트 | O(N) | O(1) | 중복 Node 탐색 후 head에 삽입 |
| Topic 등록 | 단일 연결 리스트 | O(T) | O(1) | 중복 Topic 탐색 후 head에 삽입 |
| Node 검색 | 단일 연결 리스트 | O(N) | O(1) | 이름이 같은 Node를 순차 탐색 |
| Topic 검색 | 단일 연결 리스트 | O(T) | O(1) | 이름이 같은 Topic을 순차 탐색 |
| Publisher 등록 | 연결 리스트 | O(N + T + P) | O(1) | Node, Topic, 중복 Publisher 확인 후 삽입 |
| Subscriber 등록 | 연결 리스트 | O(N + T + S) | O(1) | Node, Topic, 중복 Subscriber 확인 후 삽입 |
| Message Publish | priority queue, 연결 리스트 | O(N + T + P + M) | O(1) | Publisher 검증 후 priority 기준 정렬 삽입 |
| Message Receive | priority queue, 연결 리스트 | O(N + T + S) | O(1) | Subscriber 검증 후 queue front 제거 |
| Priority Queue 삽입 | 정렬 연결 리스트 | O(M) | O(1) | priority가 높은 Message가 앞에 오도록 삽입 |
| Priority Queue 제거 | 정렬 연결 리스트 | O(1) | O(1) | 이미 정렬된 queue의 front를 제거 |
| Communication Graph 출력 | Topic, Publisher, Subscriber 리스트 | O(T + P_total + S_total) | O(1) | 모든 Topic 관계를 간선 형태로 출력 |
| BFS Path Search | Queue, 배열, 연결 리스트 순회 | O(V * (T + S_total)) | O(V) | Node를 BFS queue에 넣고 Topic 관계를 순회하며 인접 Node 탐색 |
| Simulator Destroy | 연결 리스트 순회 | O(N + T + P_total + S_total + M_total) | O(1) | 동적 할당된 Node, Topic, Message, Publisher, Subscriber 해제 |

## 3. 분석 요약

이 프로젝트는 자료구조 수업 최종 프로젝트에 맞게 배열보다 연결 리스트 중심으로 구현했다. Node, Topic, Publisher, Subscriber는 실행 중 동적으로 추가되므로 head 삽입이 쉬운 단일 연결 리스트를 사용한다.

Message Queue는 FR-08 이후 priority queue로 동작한다. 새 메시지를 발행할 때 priority 기준으로 정렬 삽입하므로 삽입은 O(M), 수신은 queue front를 제거하므로 O(1)이다. priority 값이 같은 메시지는 기존 발행 순서를 유지한다.

Communication Graph는 별도 그래프 구조체를 중복 저장하지 않고, Topic이 가진 Publisher/Subscriber 리스트를 그래프의 간선 정보처럼 사용한다. BFS 경로 탐색도 이 관계를 직접 순회하여 수행한다. 이 방식은 구현이 단순하고 메모리 사용이 적지만, BFS 중 매 Node마다 Topic 관계를 다시 훑기 때문에 인접 리스트를 별도로 구성한 일반적인 BFS의 O(V + E)보다 비효율적일 수 있다.
