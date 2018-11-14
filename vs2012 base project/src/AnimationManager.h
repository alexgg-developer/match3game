#pragma once

#include "IAnimation.h"
#include "IDrawableAnimation.h"

#include <list>
#include <memory>

#include <glm\glm.hpp>


class AnimationManager
{
public:
	AnimationManager();
	void Add(std::unique_ptr<IAnimation> animation);
	void AddDrawable(std::unique_ptr<IDrawableAnimation> drawable);
	void Draw(King::Engine& engine);
	void Update(float lastFrameSeconds);
private:
	std::list<std::unique_ptr<IAnimation>> mCurrentAnimations;
	std::list<std::unique_ptr<IDrawableAnimation>> mCurrentDrawableAnimations;
};

