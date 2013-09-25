#pragma once
#include <RingBuffer.hpp>
#include "GLAS.hpp"

class AnalogStick{
public:
	void update(float x, float y);
	Glas::Vector2f getTilt();
private:
	TUL::RingBuffer<Glas::Vector2f,3> m_Tilt;
};