#include "MovieIrrAdapter.h"

#include <Singleton.hpp>

#include "IrrAdapter.h"
#include "murmuurVIDEO.h"

namespace IrrAdapter{
	Movie::Movie(){
		auto node = GetSingleton<IrrApp>()->accessSceneManager()->addCubeSceneNode(1, nullptr, -1, irr::core::vector3df(0,0,0), irr::core::vector3df(0,0,0), irr::core::vector3df(2, 2, 1));
		m_pNode = std::shared_ptr<irr::scene::IMeshSceneNode>(node, IrrSafeRemove());
		m_pNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		m_pNode->setMaterialTexture(0, GetSingleton<IrrApp>()->accessVideoDriver()->getTexture("blank.png"));

		m_VideoPlayer = std::shared_ptr<murmuurVIDEO>(
			new murmuurVIDEO(
				GetSingleton<IrrApp>()->accessVideoDriver(),
				GetSingleton<IrrApp>()->accessDevice()->getTimer(),
				1024,
				768,
				m_pNode.get()
			));
		m_Playing = false;
	}
	bool Movie::open(std::string name){
		return m_Playing = m_VideoPlayer->open(name.c_str());
	}
	void Movie::draw(){
		m_pNode->setVisible(true);
	}

	bool Movie::refresh(){
		if(m_Playing && m_VideoPlayer->refresh()){
			m_VideoPlayer->drawVideoTexture();
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

	Movie::~Movie(){
		//close();
	}
};