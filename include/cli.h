/**
 * @file cli.h
 * @brief ROS2 Pub/Sub C Simulator의 명령어 기반 CLI 인터페이스.
 *
 * 이 헤더는 사용자 명령어를 읽어 Simulator API로 전달하는 CLI 실행 함수를 선언한다.
 */

#ifndef CLI_H
#define CLI_H

#include "simulator.h"

/**
 * @brief 명령어 기반 CLI 루프를 실행한다.
 *
 * 사용자는 `add_node`, `add_topic`, `publish`, `receive`, `graph`, `search` 등의
 * 명령어를 입력하여 Simulator 상태를 조작할 수 있다.
 * Simulator의 초기화와 해제는 호출자가 담당한다.
 *
 * @param sim CLI에서 사용할 Simulator 포인터
 */
void cli_run(Simulator *sim);

#endif
