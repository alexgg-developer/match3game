#pragma once

#include <king\Engine.h>

#include "IAnimation.h"

class IDrawableAnimation: public IAnimation
{
public:
	virtual void Draw(King::Engine& engine) = 0;
};

