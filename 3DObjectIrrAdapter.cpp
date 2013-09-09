#include "3DObjectIrrAdapter.h"

#include <Singleton.hpp>
#include "IrrAdapter.h"


IrrAdapter::Drawer3DImpl::Drawer3DImpl(){
	std::fill(m_Scale.begin(), m_Scale.end(), 1);
	auto node = GetSingleton<IrrApp>()->accessSceneManager()->addMeshSceneNode(
		GetSingleton<IrrApp>()->accessSceneManager()->getMesh(IrrBlankXFile)
	);
	m_Node = std::shared_ptr<irr::scene::IMeshSceneNode>(node, IrrSafeRemove());
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