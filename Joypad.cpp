#include "Joypad.h"

#include <map>

#include "AnalogStick.h"
#include "Button.h"

struct AbsJoypad::Impl{
	std::map<int,Button> m_Buttons;
	AnalogStick m_LStick;
	AnalogStick m_RStick;
};

AbsJoypad::AbsJoypad(){
	__impl__ = std::make_shared<Impl>();
}

void AbsJoypad::setButton(DefaultButton name, bool b){
	__impl__->m_Buttons[name].update(b);
}

void AbsJoypad::setLStick(float x, float y){
	__impl__->m_LStick.update(x,y);
}
void AbsJoypad::setRStick(float x, float y){
	__impl__->m_RStick.update(x,y);
}
Button & AbsJoypad::getButton(DefaultButton button){
	return __impl__->m_Buttons[button];
}

AnalogStick & AbsJoypad::getLStick(){
	return __impl__->m_LStick;
}
AnalogStick &  AbsJoypad::getRStick(){
	return __impl__->m_RStick;
}
