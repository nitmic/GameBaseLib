#include "MovieIrrAdapter.h"

#include <Singleton.hpp>

#include "IrrAdapter.h"
#include "murmuurVIDEO.h"

namespace IrrAdapter{
	Movie::Movie(){
		auto node = GetSingleton<IrrApp>()->accessSceneManager()->addSphereSceneNode();
		m_pNode = std::shared_ptr<irr::scene::IMeshSceneNode>(node, IrrSafeRemove());
		m_pNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		m_pNode->setMaterialTexture(0, GetSingleton<IrrApp>()->accessVideoDriver()->getTexture("color.png"));

		m_VideoPlayer = std::shared_ptr<murmuurVIDEO>(
			new murmuurVIDEO(
				GetSingleton<IrrApp>()->accessVideoDriver(),
				GetSingleton<IrrApp>()->accessDevice()->getTimer(),
				1024,
				768,
				m_pNode.get()
			), SafeReleaseMurmuur);
		m_Playing = false;
	}
	bool Movie::open(std::string name){
		return m_Playing = m_VideoPlayer->open(name.c_str());
	}
	bool Movie::refresh(){
		if(m_Playing && m_VideoPlayer->refresh()){
			m_VideoPlayer->drawVideoTexture();
		}else{
			m_Playing = false;
		}
	}

	void Movie::close(){
		m_VideoPlayer->close();
		m_Playing = false;
	}
};