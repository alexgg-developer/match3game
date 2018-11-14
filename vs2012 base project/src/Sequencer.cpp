#include "Sequencer.h"



Sequencer::Sequencer(): mStopped(false)
{

}

bool Sequencer::HasStopped() const
{
	return mStopped;
}


void Sequencer::Add(std::unique_ptr<IAnimation> animation)
{
	mAnimations.emplace(std::move(animation));
}

#include <iostream>
using namespace std;

void Sequencer::Update(float lastFrameSeconds)
{
	if (!mAnimations.empty()) {
		auto& animation = mAnimations.front();
		if (!animation->HasStopped()) {
			animation->Update(lastFrameSeconds);
		}
		else {
			//cout << "NEW ANIMATION" << endl;
			mAnimations.pop();
		}
	}
	else {
		mStopped = true;
	}
}
