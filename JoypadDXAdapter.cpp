#include "JoypadDXAdapter.h"

#include <map>
#include <cmath>
#include <Singleton.hpp>
#include <DXJoypad.h>
#include <DXStdafx.h>

#include "Button.h"
#include "AnalogStick.h"

struct DXAdapter::Joypad::Impl{
	//std::map<AbsJoypad::DefaultButton,int> keyConfig;
};


DXAdapter::Joypad::Joypad(unsigned int playerNum) : m_PlayerNum(playerNum){
	__impl__ = std::make_shared<Impl>();
}

#pragma warning(disable:4482)
#pragma warning(disable:4800)

void DXAdapter::Joypad::update(){
	auto pad = GetSingleton<DXLib::DXJoypad>();
	setButton(DefaultButton::A,       pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::A) & 0x80);
	setButton(DefaultButton::B,       pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::B) & 0x80);
	setButton(DefaultButton::X,       pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::X) & 0x80);
	setButton(DefaultButton::Y,       pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::Y) & 0x80);
	setButton(DefaultButton::Select,  pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::Select) & 0x80);
	setButton(DefaultButton::Start,   pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::Start) & 0x80);
	setButton(DefaultButton::Up,      pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::Up) & 0x80);
	setButton(DefaultButton::Down,    pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::Down) & 0x80);
	setButton(DefaultButton::Left,    pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::Left) & 0x80);
	setButton(DefaultButton::Right,   pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::Right) & 0x80);
	setButton(DefaultButton::L1,    pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::L1) & 0x80);
	setButton(DefaultButton::R1,   pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::R1) & 0x80);
	setButton(DefaultButton::L2,    pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::L2) & 0x80);
	setButton(DefaultButton::R2,   pad->getButtonState(m_PlayerNum, DXLib::DXJoypad::R2) & 0x80);
	auto x = pad->getLAnalogStickState(m_PlayerNum) / DXLib::DXJoypad::DX_PROP_MAX;
	if(x.x==1){
		x.x = std::sqrt(1  -  x.y * x.y);
	}else if(x.y==1){
		x.y = std::sqrt(1  -  x.x * x.x);
	}else if(D3DXVec2Length(&x) > 1){
		D3DXVec2Normalize(&x, &x);
	}
	setLStick(x.x, x.y);
	auto y = pad->getRAnalogStickState(m_PlayerNum);
	if(y.x==1){
		y.x = std::sqrt(1  -  y.y * y.y);
	}else if(y.y==1){
		y.y = std::sqrt(1  -  y.x * y.x);
	}else if(D3DXVec2Length(&y) > 1){
		D3DXVec2Normalize(&y, &y);
	}
}