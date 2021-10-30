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
}

void Game::ResetGame()
{
	turn_ = true;
	ball_goals_[0] = 0;
	ball_goals_[1] = 0;
	my_ball_count_ = 0;
	first_touch_ball_ = 0;
	group_ = BallGroup::NotDecided;
	is_first_group_set_ = false;
	is_foul_ = false;
	is_turn_out_ = false;
	ball_placing_ = false;
}

void Game::SetTurn(bool turn)
{
	// get percent of ball goal count
	float percent = 0.0f;
	if (group_ != BallGroup::NotDecided)
		percent = ball_goals_[turn == (group_ == BallGroup::P1Strip)] / 8.0f;

	ui_->SetTurn(turn, group_, percent);
	if (this->turn_ != turn)
	{
		ui_->ShowMessage(MSG_TURN(turn));
		this->turn_ = turn;
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
		is_foul_ = true;
	}

	if (ball_id == 8)
	{
		const bool win = ball_goals_[turn_ == (group_ == BallGroup::P1Strip)] == 7;
		ui_->GoGameEnd(MSG_WIN(turn_));
		return;
	}

	const bool is_solid = ball_id >= 1 && ball_id <= 7;
	if (group_ == BallGroup::NotDecided)
	{
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
			my_ball_count_++;
		}
	}

	ball_goals_[!is_solid]++;
	SetTurn(turn_); // Update progress
}

void Game::OnAllBallStopped()
{
	if (ball_placing_)
		return;

	// set foul when first touch is not my ball or none;
	const bool is_solid = first_touch_ball_ >= 1 && first_touch_ball_ <= 7;
	if (first_touch_ball_ == 0 || group_ != BallGroup::NotDecided && !is_first_group_set_ && is_solid == ((group_ == BallGroup::P1Solid) != turn_))
	{
		is_foul_ = true;
		is_turn_out_ = true;
	}

	// turn change
	if (is_turn_out_ || !is_first_group_set_ && my_ball_count_ == 0)
	{
		SetTurn(!turn_);
		is_turn_out_ = false;
	}
	
	// reset temp variables
	my_ball_count_ = 0;
	is_first_group_set_ = false;
	first_touch_ball_ = 0;

	// foul
	if (is_foul_)
	{
		ui_->ShowMessage(MSG_FOUL);
		scene_->DisableCueControl();
		scene_->ResetWhiteBall();
		ball_placing_ = true;
		is_foul_ = false;
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
	return !is_foul_ && !ball_placing_;
}

bool Game::IsPlayer1Turn()
{
	return turn_;
}