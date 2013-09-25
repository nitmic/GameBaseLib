#include "3DObjectIrrAdapter.h"

#include <Singleton.hpp>
#include "IrrAdapter.h"


IrrAdapter::Drawer3DImpl::Drawer3DImpl(){
	std::fill(m_Scale.begin(), m_Scale.end(), 1);
	auto mesh = GetSingleton<IrrApp>()->accessSceneManager()->getMesh(IrrBlankXFile);
	auto node = GetSingleton<IrrApp>()->accessSceneManager()->addAnimatedMeshSceneNode(mesh);
	m_Node = std::shared_ptr<irr::scene::IAnimatedMeshSceneNode>(node, IrrSafeRemove());
	m_Node->setAnimationSpeed(50);
	m_Node->setFrameLoop(0,28);
	m_Node->setMaterialFlag(irr::video::EMF_ZBUFFER, true);
	m_Node->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, true);
	m_Node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
}

void IrrAdapter::Drawer3DImpl::setResouceName(tString name){
	if(name!=m_name){
		m_name = name;
		m_Node->setMesh(GetSingleton<IrrApp>()->accessSceneManager()->getMesh(name.c_str()));
	}
}
void IrrAdapter::Drawer3DImpl::setPosition(Glas::Vector3f pos){
	m_Pos = pos;
}
void IrrAdapter::Drawer3DImpl::setScale(float amount){
	m_Scale[0] = amount;
	m_Scale[1] = amount;
	m_Scale[2] = amount;
}

void IrrAdapter::Drawer3DImpl::setAttitude(Glas::Quaternion attitude){
	m_Attitude = attitude;
}

void IrrAdapter::Drawer3DImpl::draw(){
	m_Node->setVisible(true);

	m_Node->setScale(m_Scale);
	m_Node->setPosition(m_Pos);

	Glas::EulerAngle3r eular;
	m_Attitude.toEuler(eular);
	m_Node->setRotation(Glas::Vector3f(
		((TUL::Degree)(eular.X)).getRaw(),
		((TUL::Degree)(eular.Y)).getRaw(),
		((TUL::Degree)(eular.Z)).getRaw()
	));
}