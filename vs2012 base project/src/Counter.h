#pragma once

#include <king\Engine.h>

#include <glm\glm.hpp>

class Counter
{
public:
	Counter() = delete;
	explicit Counter(float initialCount, glm::vec2& position, float additionalTextDuration);
	void Draw(King::Engine& engine);
	bool HasStopped() const;
	void Update(float lastFrameSeconds);
private:
	float mInitialCount, mCurrentCount, mAdditionalTextDuration;
	glm::vec2 mPosition;
	glm::mat4 mTransform;
	bool mStopped;
};

