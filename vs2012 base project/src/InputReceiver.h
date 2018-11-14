#pragma once

class InputReceiver
{
public:
	virtual void OnMouseDown(float x, float y) = 0;
	virtual void OnMouseUp(float x, float y) = 0;
	virtual void OnMouseMove(float x, float y, float mouseDownX, float mouseDownY) = 0;
};
