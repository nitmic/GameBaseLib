#pragma once
#include "GLAS.hpp"
#include <Singleton.hpp>
#include "IrrAdapter.h"
#include <ICameraSceneNode.h>

namespace IrrAdapter{
	template<class T>
	class Camera : public T{
	public:
		Camera(){
			auto node = GetSingleton<IrrApp>()->accessSceneManager()->addCameraSceneNode(
				nullptr, Glas::Vector3f(0,0,-5), Glas::Vector3f(0,0,0)
			);
			m_Node = std::shared_ptr<irr::scene::ICameraSceneNode>(node, IrrSafeRemove());

			auto light = GetSingleton<IrrApp>()->accessSceneManager()->addLightSceneNode(
				m_Node.get(), Glas::Vector3f(0,0,10), irr::video::SColorf(0xFFFFFFFF), 10.0f
			);
			m_Light = std::shared_ptr<irr::scene::ILightSceneNode>(light, IrrSafeRemove());
			m_Light->setLightType(irr::video::ELT_DIRECTIONAL);
		}
		~Camera(){}
		void transform(){
			m_Node->setVisible(true);
			m_Node->setPosition(getPosition());
			
			m_Node->setTarget(getLookAt());
			m_Node->setUpVector(getAttitude() * Glas::Vector3f(0,1,0));

			m_Light->setVisible(true);
		}
	private:
		std::shared_ptr<irr::scene::ILightSceneNode> m_Light;
		std::shared_ptr<irr::scene::ICameraSceneNode> m_Node;
	};
};