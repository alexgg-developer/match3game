#include "InputControl.h"

InputControl::InputControl()
{
}

void InputControl::Register(InputReceiver& inputReceiver)
{
	mReceivers.push_back(&inputReceiver);
}

void InputControl::Update(bool isMouseDown, float x, float y)
{
	if (!mIsMouseDown && isMouseDown) {
		mMouseDownPosition = glm::vec2(x, y);
		for (auto receiver : mReceivers) {
			receiver->OnMouseDown(x, y);
		}
	}
	else if (mIsMouseDown && !isMouseDown) {
		mMouseUpPosition = glm::vec2(x, y);
		for (auto receiver : mReceivers) {
			receiver->OnMouseUp(x, y);
		}
	}
	else if (mIsMouseDown && isMouseDown) {
		for (auto receiver : mReceivers) {
			receiver->OnMouseMove(x, y, mMouseDownPosition.x, mMouseDownPosition.y);
		}
	}

	mIsMouseDown = isMouseDown;
}


