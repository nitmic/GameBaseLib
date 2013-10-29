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
}

void IrrAdapter::Drawer3DImpl::setResouceName(TUL::tString name){
	if(name!=m_name){
		m_name = name;
		m_Node->setMesh(GetSingleton<IrrApp>()->accessSceneManager()->getMesh(name.c_str()));
		m_Node->setMaterialFlag(irr::video::EMF_ANTI_ALIASING,true);
		m_Node->getMaterial(1).BackfaceCulling = false;
		m_Node->getMaterial(1).Lighting = false;
		m_Node->getMaterial(1).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(2).BackfaceCulling = false;
		m_Node->getMaterial(2).Lighting = false;
		m_Node->getMaterial(2).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(3).BackfaceCulling = false;
		m_Node->getMaterial(3).Lighting = false;
		m_Node->getMaterial(3).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(4).BackfaceCulling = false;
		m_Node->getMaterial(4).Lighting = false;
		m_Node->getMaterial(4).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(5).BackfaceCulling = false;
		m_Node->getMaterial(5).Lighting = false;
		m_Node->getMaterial(5).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(6).BackfaceCulling = false;
		m_Node->getMaterial(6).Lighting = false;
		m_Node->getMaterial(6).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(7).BackfaceCulling = false;
		m_Node->getMaterial(7).Lighting = false;
		m_Node->getMaterial(7).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(9).BackfaceCulling = false;
		m_Node->getMaterial(9).Lighting = false;
		m_Node->getMaterial(9).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(10).BackfaceCulling = false;
		m_Node->getMaterial(10).Lighting = false;
		m_Node->getMaterial(10).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(11).BackfaceCulling = false;
		m_Node->getMaterial(11).Lighting = false;
		m_Node->getMaterial(11).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(12).BackfaceCulling = false;
		m_Node->getMaterial(12).Lighting = false;
		m_Node->getMaterial(12).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(13).BackfaceCulling = false;
		m_Node->getMaterial(13).Lighting = false;
		m_Node->getMaterial(13).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(14).BackfaceCulling = false;
		m_Node->getMaterial(14).Lighting = false;
		m_Node->getMaterial(14).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		m_Node->getMaterial(15).BackfaceCulling = false;
		m_Node->getMaterial(15).Lighting = false;
		m_Node->getMaterial(15).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
		//m_Node->setMaterialFlag(irr::video::EMF_ZBUFFER, true);
		//m_Node->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, true);
		m_Node->setMaterialFlag(irr::video::EMF_BILINEAR_FILTER,false);
		//auto gpu = GetSingleton<IrrApp>()->accessVideoDriver()->getGPUProgrammingServices();
		//auto celMat = gpu->addHighLevelShaderMaterialFromFiles(
		//	"cel.hlsl", "vertexMain", irr::video::EVST_VS_2_0,
		//	"cel.hlsl", "pixelMain", irr::video::EPST_PS_2_0, this);
		//m_Node->setMaterialType((irr::video::E_MATERIAL_TYPE)celMat);
	}
}
void IrrAdapter::Drawer3DImpl::setPosition(Glas::Vector3f pos){
	m_Pos = pos;
}
void IrrAdapter::Drawer3DImpl::setScale(float amount){
	m_Scale[0] = amount;
	m_Scale[1] = amount;
	m_Scale[2] = amount;
	m_Node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS,true);
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