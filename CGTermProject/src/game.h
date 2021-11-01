#pragma once
#include "guiscreen.h"

class Scene;
class Game
{
public:
	Game(Scene* scene_, GuiScreen* ui_);
	void ResetGame();
	void SetTurn(bool is_p1_turn);
	bool IsBallPlacingMode();
	bool CanInteractWhiteBall();
	bool IsPlayer1Turn();
	virtual void OnWhiteBallCollide(int ball_id);
	virtual void OnBallHoleIn(int ball_id);
	virtual void OnAllBallStopped();
	void OnBallPlaced();

private:
	Scene* scene_;
	GuiScreen* ui_;

	// game variables
	bool turn_; // true = player1
	int ball_goals_[2]; // count of goal balls ([0] solid, [1] stripe)
	int turn_goal_ball_count_; // 이번 턴에 넣은 나의 공(solid or stripe)의 개수
	int first_touch_ball_; // white ball과 닿은 최초의 ball id
	BallGroup group_;
	bool is_first_group_set_; // 이번턴에서 group이 결정되었는가?
	bool is_foul_next_;
	bool is_turn_out_;
	bool ball_placing_; // 파울이어서 흰색 공의 위치를 옮기고 있는가?
};