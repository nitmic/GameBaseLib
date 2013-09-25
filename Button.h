#pragma once
#include <RingBuffer.hpp>

class Button{
public:
	void update(bool b);
	bool isJustPressed();
	bool isJustPull();
	bool isPressed();
	bool isFree();
private:
	TUL::RingBuffer<bool,2> m_State;
};