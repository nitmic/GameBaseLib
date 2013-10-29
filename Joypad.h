#pragma once
#include <memory>
#include <tString.hpp>

class AnalogStick;
class Button;

class AbsJoypad{
public:
	virtual void update() = 0;
	
	AbsJoypad();
	virtual ~AbsJoypad() = 0 {};

	enum DefaultButton{
		A, B, X, Y, L1, L2, R1, R2, Select, Start, Up, Right, Down, Left
	};
	AnalogStick & getLStick();
	AnalogStick & getRStick();
	Button & getButton(DefaultButton button);
	void applyKeyConfig(TUL::tString filename);
protected:
	void setLStick(float x, float y);
	void setRStick(float x, float y);
	void setButton(DefaultButton name, bool b);
private:
	struct Impl;
	std::shared_ptr<Impl> __impl__;
};