#pragma once
#include <string>

// Player name macros
#define PLAYER1_NAME					L"Player 1"
#define PLAYER2_NAME					L"Player 2"
#define GET_PLAYER_NAME(p1_turn)		((p1_turn) ? PLAYER1_NAME : PLAYER2_NAME)

// Ball type macros
#define BALL_STRIPED_NAME				L"�ٹ��� ��"
#define BALL_SOLID_NAME					L"�ܻ� ��"
#define GET_BALL_TYPE(is_solid)			((is_solid) ? BALL_SOLID_NAME : BALL_STRIPED_NAME)

// UI texts
#define TEXT_BTN_START_GAME				L"���� ����"
#define TEXT_BTN_HOW_TO_PLAY			L"���� ���"
#define TEXT_BTN_EXIT					L"����"
#define TEXT_BTN_TO_MAIN				L"���� �޴���"

// Messages
#define MSG_VIEW_CENTER_BALL			L"���� �߽����� ����"
#define MSG_VIEW_CENTER_TABLE			L"���̺��� �߽����� ����"
#define MSG_TURN(p1_turn)				std::wstring(GET_PLAYER_NAME(p1_turn)) + L" �����Դϴ�!"
#define MSG_CANNOT_PLACE_THERE			L"�� ��ġ�� ��ġ�� �� �����ϴ�."
#define MSG_WIN(p1_turn)				std::wstring(GET_PLAYER_NAME(p1_turn)) + L"��(��) �̰���ϴ�."
#define MSG_DECIDED_BALL(p1_turn, type)	std::wstring(GET_PLAYER_NAME(p1_turn)) + L"��(��) ������ " + std::wstring(GET_BALL_TYPE(type)) + L"�� �־���մϴ�."
#define MSG_FOUL						L"�Ŀ��Դϴ�. ������� ��ġ�ϼ���."