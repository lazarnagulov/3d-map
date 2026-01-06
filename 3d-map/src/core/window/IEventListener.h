#pragma once

class IEventListener {
public:
	virtual void OnKey(int key, int action) = 0;
	virtual void OnMouseMove(double x, double y) = 0;
	virtual void OnMouseButton(int button, int action) = 0;
};