#pragma once
#include <tString.hpp>
#include <GLAS.hpp>

namespace irr{
	namespace video{
		class ITexture;
	};
};

namespace IrrAdapter{
	/*
	*@class IrrAdapter::Image
	*  �����̑����B�Ӗ��͖���
	*/
	class Image{
	public:
		typedef irr::video::ITexture * Raw;

		Image() : tex(nullptr){};
		Image(TUL::tString name);
		Image(Raw raw);
		TUL::tString getName();
		int getHeight();
		int getWidth();

		Raw getRaw();
	private:
		Raw tex;
	};
};