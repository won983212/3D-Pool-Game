#pragma once
#include <string>

// Player name macros
#define PLAYER1_NAME					L"Player 1"
#define PLAYER2_NAME					L"Player 2"
#define GET_PLAYER_NAME(p1_turn)		((p1_turn) ? PLAYER1_NAME : PLAYER2_NAME)

// Ball type macros
#define BALL_STRIPED_NAME				L"줄무늬 공"
#define BALL_SOLID_NAME					L"단색 공"
#define GET_BALL_TYPE(is_solid)			((is_solid) ? BALL_SOLID_NAME : BALL_STRIPED_NAME)

// UI texts
#define TEXT_BTN_START_GAME				L"게임 시작"
#define TEXT_BTN_HOW_TO_PLAY			L"게임 방법"
#define TEXT_BTN_EXIT					L"종료"
#define TEXT_BTN_TO_MAIN				L"메인 메뉴로"

// Messages
#define MSG_VIEW_CENTER_BALL			L"공을 중심으로 보기"
#define MSG_VIEW_CENTER_TABLE			L"테이블을 중심으로 보기"
#define MSG_TURN(p1_turn)				std::wstring(GET_PLAYER_NAME(p1_turn)) + L" 차례입니다!"
#define MSG_CANNOT_PLACE_THERE			L"그 위치에 배치할 수 없습니다."
#define MSG_WIN(p1_turn)				std::wstring(GET_PLAYER_NAME(p1_turn)) + L"이(가) 이겼습니다."
#define MSG_DECIDED_BALL(p1_turn, type)	std::wstring(GET_PLAYER_NAME(p1_turn)) + L"은(는) 앞으로 " + std::wstring(GET_BALL_TYPE(type)) + L"을 넣어야합니다."
#define MSG_FOUL						L"파울입니다. 흰색공을 배치하세요."