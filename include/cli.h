#ifndef CLI_H
#define CLI_H

#include "simulator.h"

/**
 * @brief ROS2 Pub/Sub C Simulator의 메뉴 기반 CLI 루프를 실행한다.
 *
 * 사용자의 메뉴 입력을 받아 Node 등록, Topic 등록, Publisher 등록,
 * Subscriber 등록, Message Publish, 등록 목록 출력 기능을 수행한다.
 *
 * 이 함수는 Simulator의 초기화나 해제를 담당하지 않는다.
 * Simulator의 생명주기는 호출자가 관리해야 한다.
 *
 * @param sim CLI에서 사용할 Simulator 포인터
 */
void cli_run(Simulator *sim);

#endif
