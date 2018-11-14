#include "AnimationManager.h"



AnimationManager::AnimationManager()
{
}



void AnimationManager::Add(std::unique_ptr<IAnimation> animation)
{	
	mCurrentAnimations.emplace_back(std::move(animation));	
}

void AnimationManager::AddDrawable(std::unique_ptr<IDrawableAnimation> animation)
{
	mCurrentDrawableAnimations.emplace_back(std::move(animation));
}


void AnimationManager::Update(float lastFrameSeconds)
{
	for (auto it = mCurrentAnimations.begin(); it != mCurrentAnimations.end(); ) {
		if (!(*it)->HasStopped()) {
			(*it)->Update(lastFrameSeconds);
			++it;
		}
		else {
			it = mCurrentAnimations.erase(it);
		}
	}	

	for (auto it = mCurrentDrawableAnimations.begin(); it != mCurrentDrawableAnimations.end(); ) {
		if (!(*it)->HasStopped()) {
			(*it)->Update(lastFrameSeconds);
			++it;
		}
		else {
			it = mCurrentDrawableAnimations.erase(it);
		}
	}
}



void AnimationManager::Draw(King::Engine& engine)
{
	for (auto it = mCurrentDrawableAnimations.begin(); it != mCurrentDrawableAnimations.end(); ) {
		(*it++)->Draw(engine);
	}
}

