#pragma once

#include "IDrawableAnimation.h"

#include <glm\glm.hpp>

#include <string>

#include <king\Engine.h>

class TextAnimation : public IDrawableAnimation
{
public:
	TextAnimation() = delete;
	TextAnimation(std::string& text, glm::vec2 startPosition, glm::vec2 movement, float duration);
	virtual void Update(float lastFrameSeconds) override;
	bool HasStopped() const final override;
	void Draw(King::Engine& engine) override;
private:
	glm::vec2 mStartPosition, mMovement, mCurrentPosition;
	float mDuration, mElapsed;
	std::string mText;
	bool mStopped;

	
};

