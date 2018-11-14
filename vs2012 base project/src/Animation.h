#pragma once

#include "Gem.h"
#include "IAnimation.h"

class Animation: public IAnimation
{
public:
	Animation(Gem& node);
	virtual void Update(float lastFrameSeconds) override {};	
	bool HasStopped() const override final;
protected:
	Gem& mNode;
	bool mStopped;
};
