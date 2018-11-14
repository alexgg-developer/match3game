#pragma once

#include "Animation.h"

#include <iostream>

#include <glm\glm.hpp>

class MoveBy : public Animation
{
public:
	MoveBy(glm::vec2& movement, float duration, Gem& node);
	void Update(float lastFrameSeconds) override;
private:
	float mDuration, mElapsed;
	glm::vec2 mMovement;
	glm::vec2 mStartPosition, mPreviousPosition;

};

