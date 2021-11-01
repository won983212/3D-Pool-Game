#include <iostream>
#include "game.h"
#include "gameenum.h"
#include "strings.h"
#include "scene.h"

Game::Game(Scene* scene, GuiScreen* ui)
{
	if (scene == nullptr || ui == nullptr)
	{
		std::cout << "Error: scene, ui can't be nullptr" << std::endl;
		throw;
	}

	scene_ = scene;
	ui_ = ui;
	ResetGame();
}

void Game::ResetGame()
{
	turn_ = true;
	ball_goals_[0] = 0;
	ball_goals_[1] = 0;
	turn_goal_ball_count_ = 0;
	first_touch_ball_ = 0;
	group_ = BallGroup::NotDecided;
	is_first_group_set_ = false;
	is_foul_next_ = false;
	is_turn_out_ = false;
	ball_placing_ = false;
}

void Game::SetTurn(bool is_p1_turn)
{
	// get percent of ball goal count
	float percent = 0.0f;
	if (group_ != BallGroup::NotDecided)
		percent = ball_goals_[is_p1_turn == (group_ == BallGroup::P1Strip)] / 8.0f;

	// change ui state
	ui_->SetTurn(is_p1_turn, group_, percent);
	if (this->turn_ != is_p1_turn)
	{
		ui_->ShowMessage(MSG_TURN(is_p1_turn));
		this->turn_ = is_p1_turn;
	}
}

void Game::OnWhiteBallCollide(int ball_id)
{
	if (first_touch_ball_ == 0)
	{
		first_touch_ball_ = ball_id;
	}
}

void Game::OnBallHoleIn(int ball_id)
{
	// foul. white ball is in hole.
	if (ball_id == 0)
	{
		is_foul_next_ = true;
		return;
	}

	// game end.
	if (ball_id == 8)
	{
		const bool win = ball_goals_[turn_ == (group_ == BallGroup::P1Strip)] == 7;
		ui_->GoGameEnd(MSG_WIN(turn_));
		return;
	}

	const bool is_solid = ball_id >= 1 && ball_id <= 7;
	if (group_ == BallGroup::NotDecided)
	{
		// solid인지 stripe인지 결정.
		group_ = turn_ == is_solid ? BallGroup::P1Solid : BallGroup::P1Strip;
		ui_->ShowMessage(MSG_DECIDED_BALL(turn_, is_solid));
		is_first_group_set_ = true;
	}
	else
	{
		if (is_solid == (group_ == BallGroup::P1Solid) != turn_)
		{
			is_turn_out_ = true;
		}
		else
		{
			turn_goal_ball_count_++;
		}
	}

	ball_goals_[!is_solid]++;
	SetTurn(turn_); // for updating progress
}

void Game::OnAllBallStopped()
{
	if (ball_placing_)
		return;

	// foul check
	// 첫 공 터치가 내 공이 아니거나, 아무 공도 못쳤으면 파울
	// * 아직 공 타입이 안 정해졌다면 봐줌
	const bool is_solid = first_touch_ball_ >= 1 && first_touch_ball_ <= 7;
	const bool my_ball_touch = is_solid != ((group_ == BallGroup::P1Solid) != turn_);
	if (first_touch_ball_ == 0 || group_ != BallGroup::NotDecided && !is_first_group_set_ && !my_ball_touch)
	{
		is_foul_next_ = true;
		is_turn_out_ = true;
	}

	// turn change
	// 다른 사람 공을 넣거나, 내 공을 하나도 못넣거나, foul이면 turn change
	if (is_turn_out_ || !is_first_group_set_ && turn_goal_ball_count_ == 0)
	{
		SetTurn(!turn_);
		is_turn_out_ = false;
	}
	
	// reset temp variables
	turn_goal_ball_count_ = 0;
	is_first_group_set_ = false;
	first_touch_ball_ = 0;

	// foul
	if (is_foul_next_)
	{
		ui_->ShowMessage(MSG_FOUL);
		scene_->DisableCueControl();
		scene_->ResetWhiteBall();
		ball_placing_ = true;
		is_foul_next_ = false;
	}
	else
	{
		scene_->EnableCueControl();
	}
}

void Game::OnBallPlaced()
{
	ball_placing_ = false;
}

bool Game::IsBallPlacingMode()
{
	return ball_placing_;
}

bool Game::CanInteractWhiteBall()
{
	return !is_foul_next_ && !ball_placing_;
}

bool Game::IsPlayer1Turn()
{
	return turn_;
}