# 04. 인터페이스 명세

## 1. 메뉴 기반 인터페이스

프로그램 실행 시 다음 메뉴를 출력한다.

1. Add Node
2. Add Topic
3. Add Publisher
4. Add Subscriber
5. Publish Message
6. Receive Message
7. Print Topic Info
8. Print Communication Graph
9. Search Path Between Nodes
0. Exit

## 2. 입력 예시

Add Node:
- Input: lidar_node

Add Topic:
- Input: /scan

Add Publisher:
- Node: lidar_node
- Topic: /scan

Publish Message:
- Node: lidar_node
- Topic: /scan
- Message: range data
- Priority: 5

## 3. 출력 예시

Message published:
Topic: /scan
Publisher: lidar_node
Data: range data
Priority: 5