# 06. 복잡도 분석

| 기능 | 자료구조 | 시간복잡도 | 공간복잡도 |
|---|---|---|---|
| Node 등록 | 연결 리스트 | O(1) 또는 O(n) | O(1) |
| Topic 탐색 | 연결 리스트 | O(T) | O(1) |
| Publisher 등록 | 연결 리스트 | O(P) | O(1) |
| Subscriber 등록 | 연결 리스트 | O(S) | O(1) |
| FIFO enqueue | 큐 | O(1) | O(1) |
| FIFO dequeue | 큐 | O(1) | O(1) |
| Priority enqueue | 정렬 연결 리스트 | O(M) | O(1) |
| Priority dequeue | 정렬 연결 리스트 | O(1) | O(1) |
| Graph BFS | 인접 리스트 그래프 | O(V + E) | O(V) |

## 분석
본 프로젝트는 실행 중 Node, Topic, Message가 동적으로 추가되므로 배열보다 연결 리스트 기반 구조가 적합하다. 메시지 처리에는 FIFO Queue를 기본으로 사용하고, QoS priority 기능에서는 정렬 연결 리스트 기반 우선순위 큐를 사용한다. 전체 통신 관계는 그래프로 표현하여 BFS 탐색을 수행한다.