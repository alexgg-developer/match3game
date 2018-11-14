#include "Caller.h"

Caller::Caller(const std::function<void()> &function) :
	mFunction(function),
	mHasStopped(false)
{

}

bool Caller::HasStopped() const
{
	return mHasStopped;
}

void Caller::Update(float lastFrameSeconds)
{
	mFunction();
	mHasStopped = true;
}




