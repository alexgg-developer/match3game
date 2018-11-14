#include "MoveBy.h"

#include <algorithm>

MoveBy::MoveBy(glm::vec2& movement, float duration, Gem& node): 
	Animation(node),
	mMovement(movement),
	mDuration(duration),
	mElapsed(0.0f),
	mStartPosition(node.mPosition),
	mPreviousPosition(node.mPosition)
{
	assert(duration >= 0.0001f);
}

#include <iostream>
using namespace std;

void MoveBy::Update(float lastFrameSeconds)
{
	mStopped = mElapsed >= mDuration;
	float uniformT = mElapsed / mDuration;
	uniformT = std::min(uniformT, 1.0f);
	auto currentPos = mNode.mPosition;
	auto diff = currentPos - mPreviousPosition;
	mStartPosition = mStartPosition + diff;
	auto newPosition = mStartPosition + (mMovement * uniformT);

	//cout << "NEW newPosition" << newPosition.y << endl;
	mNode.mPosition = newPosition;
	mPreviousPosition = newPosition;


	mElapsed += lastFrameSeconds;
}

