# 01. 요구사항 명세서

## 1. 기능 요구사항

### FR-01. Node 등록
사용자는 새로운 Node 이름을 입력하여 시스템에 등록할 수 있어야 한다.

### FR-02. Topic 등록
사용자는 새로운 Topic 이름을 입력하여 시스템에 등록할 수 있어야 한다.

### FR-03. Publisher 등록
특정 Node를 특정 Topic의 Publisher로 등록할 수 있어야 한다.

### FR-04. Subscriber 등록
특정 Node를 특정 Topic의 Subscriber로 등록할 수 있어야 한다.

### FR-05. Message Publish
Publisher Node는 특정 Topic에 Message를 발행할 수 있어야 한다.

### FR-06. Message Queue
각 Topic은 발행된 Message를 내부 Queue에 저장해야 한다.

### FR-07. Message Receive
Subscriber Node는 자신이 구독한 Topic의 Message를 수신할 수 있어야 한다.

### FR-08. QoS Priority
Message는 priority 값을 가질 수 있으며, priority mode에서는 높은 priority의 메시지가 먼저 처리되어야 한다.

### FR-09. Communication Graph
Node 간 publish/subscribe 관계를 그래프로 출력할 수 있어야 한다.

### FR-10. Path Search
특정 Node에서 다른 Node까지 메시지 전달 경로가 존재하는지 BFS로 탐색할 수 있어야 한다.

## 2. 비기능 요구사항

### NFR-01. Language
C언어만 사용한다.

### NFR-02. Build
Makefile로 빌드 가능해야 한다.

### NFR-03. Memory
동적 메모리 할당 사용 시 free 함수를 통해 해제해야 한다.

### NFR-04. Error Handling
존재하지 않는 Node, Topic, Publisher, Subscriber에 대한 예외 처리를 수행해야 한다.

### NFR-05. Simplicity
자료구조 수업 기말 프로젝트 수준에 맞게 단순하고 설명 가능한 구조로 구현한다.