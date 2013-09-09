#include "Button.h"

void Button::update(bool b){
	m_State.update(b);
}

bool Button::isJustPressed(){
	if(m_State.current() && (!m_State.prev())) return true;
	return false;
}

bool Button::isJustPull(){
	if((!m_State.current()) && m_State.prev()) return true;
	return false;
}

bool Button::isPressed(){
	if(m_State.current()) return true;
	return false;
}

bool Button::isFree(){
	if(!m_State.current()) return true;
	return false;
}
