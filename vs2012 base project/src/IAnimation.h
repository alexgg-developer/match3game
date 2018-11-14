#pragma once

class IAnimation
{
public:
	virtual void Update(float lastFrameSeconds) = 0;
	virtual bool HasStopped() const = 0;
};

