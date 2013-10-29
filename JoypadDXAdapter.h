#pragma once
#include "Joypad.h"
#include <tString.hpp>

namespace DXAdapter{
	class Joypad : public AbsJoypad{
	public:
		void update();
		Joypad(unsigned int playerNum);
		
	private:
		struct Impl;
		std::shared_ptr<Impl> __impl__;

		unsigned int m_PlayerNum;
	};
};