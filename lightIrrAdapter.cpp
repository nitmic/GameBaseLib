#include "lightIrrAdapter.h"

#include "ILightSceneNode.h"

#include <Singleton.hpp>
#include "IrrAdapter.h"

namespace IrrAdapter{
	Light::Light() : m_pNode(
		GetSingleton<IrrApp>()->accessSceneManager()->addLightSceneNode(
			nullptr, Glas::Vector3f(0,0,-10), irr::video::SColorf(0xFFFFFFFF),10.0f
		)
	){
		m_pNode->setLightType(irr::video::ELT_DIRECTIONAL);
	}
	Light::~Light(){}
	void Light::setLightDirection(Glas::EulerAngle3r d){
		m_pNode->setRotation(Glas::Vector3f(
			((TUL::Degree)(d.X)).getRaw(),
			((TUL::Degree)(d.Y)).getRaw(),
			((TUL::Degree)(d.Z)).getRaw()
		));
	}
	void Light::setLightPosition(Glas::Vector3f p){
		m_pNode->setPosition(p);
	}
	void Light::draw(){
		m_pNode->setVisible(true);
	}
	void Light::draw(Glas::EulerAngle3r d, Glas::Vector3f p){
		setLightDirection(d);
		setLightPosition(p);
		draw();
	}
};