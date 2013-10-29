#include "BackgroundIrrAdapter.h"

#include <Singleton.hpp>
#include "IrrAdapter.h"

namespace IrrAdapter{
	Background::Background(){}
	void Background::setResouceName(
		TUL::tString top, TUL::tString bottom, TUL::tString left, TUL::tString right, TUL::tString front, TUL::tString back
	){
		if(
			top==m_Names[0]
			&& bottom==m_Names[1]
			&& left==m_Names[2]
			&& right==m_Names[3]
			&& front==m_Names[4]
			&& back==m_Names[5]
		) return;
		
	    GetSingleton<IrrApp>()->accessVideoDriver()->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);

		auto node =  GetSingleton<IrrApp>()->accessSceneManager()->addSkyBoxSceneNode(
			GetSingleton<IrrApp>()->accessVideoDriver()->getTexture(top.c_str()),
			GetSingleton<IrrApp>()->accessVideoDriver()->getTexture(bottom.c_str()),
			GetSingleton<IrrApp>()->accessVideoDriver()->getTexture(left.c_str()),
			GetSingleton<IrrApp>()->accessVideoDriver()->getTexture(right.c_str()),
			GetSingleton<IrrApp>()->accessVideoDriver()->getTexture(front.c_str()),
			GetSingleton<IrrApp>()->accessVideoDriver()->getTexture(back.c_str())
		);
		m_Node = std::shared_ptr<irr::scene::ISceneNode>(node, IrrSafeRemove());
		
		
	    GetSingleton<IrrApp>()->accessVideoDriver()->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);

		m_Names[0] = top;
		m_Names[1] = bottom;
		m_Names[2] = left;
		m_Names[3] = right;
		m_Names[4] = front;
		m_Names[5] = back;
	}

	void Background::draw(){
		m_Node->setVisible(true);
	}
};