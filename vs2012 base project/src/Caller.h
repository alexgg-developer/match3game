#pragma once
#include "IAnimation.h"
#include <functional>

class Caller :
	public IAnimation
{
public:
	Caller(const std::function<void()> &function);
	bool HasStopped() const override final;
	void Update(float lastFrameSeconds) override;
private:
	std::function<void()> mFunction;
	bool mHasStopped;
};

