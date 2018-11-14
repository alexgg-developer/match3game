#pragma once

#define GLM_FORCE_RADIANS 

#include <king/Engine.h>
#include <king/Updater.h>
#include <glm/glm.hpp>

#include "Grid.h"
#include "InputControl.h"
#include "TimeCounter.h"
#include "ScoreCounter.h"
#include "Counter.h"

#include <memory>

//**********************************************************************
class AnimationManager;

class TestGame : public King::Updater 
{
public:
	TestGame();
	void Start();
	void Update() override;
private:
	glm::mat4 mTransformBackground;

	TimeCounter mTimeCounter;
	King::Engine mEngine;
	InputControl mInputControl;
	std::shared_ptr<AnimationManager> mAnimationManager;
	Grid mGrid;
	ScoreCounter mScoreCounter;
	Counter mCounter;
};