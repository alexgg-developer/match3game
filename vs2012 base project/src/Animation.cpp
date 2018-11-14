#include "Animation.h"

Animation::Animation(Gem & node): mNode(node), mStopped(false)
{
}

bool Animation::HasStopped() const
{
	return mStopped;
}
