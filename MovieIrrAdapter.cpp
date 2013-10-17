#include "MovieIrrAdapter.h"

#include <Singleton.hpp>

#include "IrrAdapter.h"
#include "murmuurVIDEO.h"
#include "ImageIrrAdapter.h"

namespace IrrAdapter{
	Movie::Movie(int w, int h){
		auto node = GetSingleton<IrrApp>()->accessSceneManager()->addCubeSceneNode(1, nullptr, -1, irr::core::vector3df(0,0,0), irr::core::vector3df(0,0,0), irr::core::vector3df(2, 2, 1));
		m_VideoPlayer = std::make_shared<murmuurVIDEO>(
			GetSingleton<IrrApp>()->accessVideoDriver(),
			GetSingleton<IrrApp>()->accessDevice()->getTimer(),
			w,
			h
		);
		m_Playing = false;
	}

	void Movie::changeDecodeSize(int w, int h){
		m_VideoPlayer->changeResolution(w,h);
	}

	bool Movie::open(std::string name){
		return m_Playing = m_VideoPlayer->open(name.c_str());
	}
	Image Movie::decode(){
		return m_VideoPlayer->decode();
	}

	bool Movie::refresh(){
		if(m_Playing && m_VideoPlayer->refresh()){
			return true;
		}
		m_Playing = false;
		return false;
	}

	void Movie::close(){
		if(!m_Playing) return;
		m_VideoPlayer->close();
		m_Playing = false;
	}

	Movie::~Movie(){}
};