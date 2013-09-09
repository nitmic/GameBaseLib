#pragma once
#include <memory>

#include <tString.h>
#include "GLAS.hpp"

namespace irr{
	namespace scene{
		class IMeshSceneNode;
	};
};


namespace IrrAdapter{
	class Drawer3DImpl{
	private:
		std::shared_ptr<irr::scene::IMeshSceneNode> m_Node;
		tString m_name;
		Glas::Vector3f m_Pos;
		Glas::Quaternion m_Attitude;
		Glas::Vector3f m_Scale;
	public:
		Drawer3DImpl();
		~Drawer3DImpl(){}
		void setPosition(Glas::Vector3f pos);
		void setAttitude(Glas::Quaternion attitude);
		void setScale(float amount);
		void setResouceName(tString name);
		void draw();
	};
};