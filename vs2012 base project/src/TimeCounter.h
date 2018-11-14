#pragma once

#include<king\Engine.h>

#include <algorithm>
#include <string>

#include <glm\glm.hpp>


class TimeCounter
{
public:
	TimeCounter();
	void Draw(King::Engine& engine);
	bool HasFinished() const;
	void Update(float dt);
private:
	static constexpr int GAME_TIME = 60;

	glm::mat4 mTransform;
	float mElapsed;
	int mCurrentTime;
	bool mHasFinished;
};

