#pragma once
#include "IAnimation.h"

#include <queue>
#include <memory>

class Sequencer : public IAnimation
{
public:
	Sequencer();
	void Add(std::unique_ptr<IAnimation> animation);
	bool HasStopped() const override final;
	void Update(float lastFrameSeconds) override;

private:
	bool mStopped;
	std::queue<std::unique_ptr<IAnimation>> mAnimations;
};

