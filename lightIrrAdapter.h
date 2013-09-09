#pragma once
#include <memory>
#include "GLAS.hpp"

namespace irr{
	namespace scene{
		class ILightSceneNode;
	};
};

namespace IrrAdapter{
	class Light{
	public:
		enum Type{Point ,Spot, Directional};
		Light();
		~Light();
		void setLightDirection(Glas::EulerAngle3r d);
		void setLightPosition(Glas::Vector3f p);
		void draw();
		void draw(Glas::EulerAngle3r d, Glas::Vector3f p);
	private:
		std::shared_ptr<irr::scene::ILightSceneNode> m_pNode;
	};
};