#include "AnalogStick.h"


void AnalogStick::update(float x, float y){
	m_Tilt.update(Glas::Vector2f(x,y));
}

Glas::Vector2f AnalogStick::getTilt(){
	return m_Tilt.current();
}
