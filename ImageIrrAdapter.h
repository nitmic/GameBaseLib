#pragma once
#include <tString.h>
#include <GLAS.hpp>

namespace irr{
	namespace video{
		class ITexture;
	};
};

namespace IrrAdapter{
	class Image{
	public:
		typedef irr::video::ITexture * Raw;
		Image(tString name);
		Image(Raw raw);
		tString getName();
		int getHeight();
		int getWidth();

		Raw getRaw();
	private:
		Raw tex;
	};
};