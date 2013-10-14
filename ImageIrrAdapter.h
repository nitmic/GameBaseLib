#pragma once
#include <tString.h>

namespace irr{
	namespace video{
		class ITexture;
	};
};

namespace IrrAdapter{
	class Image{
	public:
		Image(tString name);
		irr::video::ITexture * getRaw();
	private:
		irr::video::ITexture * tex;
	};
};