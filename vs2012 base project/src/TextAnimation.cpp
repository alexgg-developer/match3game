#include "TextAnimation.h"

#include <algorithm>

TextAnimation::TextAnimation(std::string& text, glm::vec2 startPosition, glm::vec2 movement, float duration): 
	mStartPosition(startPosition),
	mMovement(movement),
	mCurrentPosition(glm::vec2(0.0f, 0.0f)),
	mDuration(duration),
	mElapsed(0.0f),
	mText(text),
	mStopped(false)
{

}

void TextAnimation::Update(float lastFrameSeconds)
{
	mStopped = mElapsed >= mDuration;
	float uniformT = mElapsed / mDuration;
	uniformT = std::min(uniformT, 1.0f);
	auto newPosition = mStartPosition + (mMovement * uniformT);
	
	mCurrentPosition = newPosition;
	mElapsed += lastFrameSeconds;
}

bool TextAnimation::HasStopped() const
{
	return mStopped;
}

void TextAnimation::Draw(King::Engine & engine)
{
	engine.Write(mText.c_str(), mCurrentPosition.x, mCurrentPosition.y);
}

