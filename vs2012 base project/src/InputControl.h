#pragma once

#include<glm\glm.hpp>

#include <vector>

#include "InputReceiver.h"

class InputControl
{
public:
	InputControl();
	void InputControl::Register(InputReceiver& inputReceiver);
	void Update(bool isMouseDown, float x, float y);
private:
	glm::vec2 mMouseDownPosition, mMouseUpPosition;
	std::vector<InputReceiver*> mReceivers;
	bool mIsMouseDown;
};

