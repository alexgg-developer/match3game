#include "Counter.h"

#include <string>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

Counter::Counter(float initialCount, glm::vec2& position, float additionalTextDuration):
	mInitialCount(initialCount),
	mPosition(position),
	mAdditionalTextDuration(additionalTextDuration),
	mStopped(false)
{
}

void Counter::Draw(King::Engine & engine)
{
	std::string text;
	if ((mInitialCount - mCurrentCount) >= 0.0f) {
		int count = std::max(static_cast<int>(mInitialCount - mCurrentCount) , 0);
		 text = std::to_string(count);
	}
	else {
		text = "Start!";
	}

	auto transform =
		glm::scale(
			glm::translate(glm::mat4(1.0), glm::vec3(mPosition.x - engine.CalculateStringWidth(text.c_str()), mPosition.y, 0.0f)),
			glm::vec3(3.0f, 3.0f, 1.0f));
	engine.Write(text.c_str(), transform);
}

bool Counter::HasStopped() const
{
	return mStopped;
}

void Counter::Update(float lastFrameSeconds)
{
	mCurrentCount += lastFrameSeconds;

	mStopped = (mInitialCount + mAdditionalTextDuration - mCurrentCount) <= 0.0f;
}
