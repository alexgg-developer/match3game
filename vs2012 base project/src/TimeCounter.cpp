#include "TimeCounter.h"

#include <glm/gtc/matrix_transform.hpp>

TimeCounter::TimeCounter():
	mElapsed(0.0f),
	mCurrentTime(GAME_TIME),
	mTransform(glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(125.0f, 429.0f, 0.0f)), glm::vec3(2.0f, 2.0f, 1.0f))),
	mHasFinished(false)
{
}

void TimeCounter::Update(float dt)
{
	if (mCurrentTime > 0) {
		mElapsed += dt;
		mCurrentTime = std::max(0, GAME_TIME - static_cast<int>(mElapsed));
	}
	else {
		mHasFinished = true;
	}
}

void TimeCounter::Draw(King::Engine& engine)
{
	constexpr float LEFT_MARGIN = 125.0f;
	constexpr float TOP_MARGIN = 429.0f;

	static const glm::vec3 SCALE_SCORE_TEXT = glm::vec3(2.0f, 2.0f, 1.0f);

	auto time = std::to_string(mCurrentTime);
	mTransform = glm::scale(glm::translate(glm::mat4(1.0), 
		glm::vec3(LEFT_MARGIN - engine.CalculateStringWidth(time.c_str()), TOP_MARGIN, 0.0f)), 
		SCALE_SCORE_TEXT);
	engine.Write(time.c_str(), mTransform);
}

bool TimeCounter::HasFinished() const
{
	return mHasFinished;
}

