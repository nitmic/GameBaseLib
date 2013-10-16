#include "ImageIrrAdapter.h"

#include <Singleton.hpp>
#include "IrrAdapter.h"

namespace IrrAdapter{
	Image::Image(tString name){
		tex = GetSingleton<IrrApp>()->accessVideoDriver()->getTexture(name.c_str());
	}
	Image::Image(Raw raw) : tex(raw){}
	
	tString Image::getName(){
		if(tex==nullptr) return _T("");
		auto name = tex->getName().getPath();
		return tString(name.c_str(), name.c_str() + name.size());
	}

	int Image::getHeight(){
		if(tex==nullptr) return 0;
		auto size = tex->getSize();
		return size.Height;
	}

	int Image::getWidth(){
		if(tex==nullptr) return 0;
		auto size = tex->getSize();
		return size.Width;
	}

	Image::Raw Image::getRaw(){
		return tex;
	}
}