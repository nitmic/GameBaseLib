#pragma once
#include <memory>
#include <array>
#include <tString.h>


namespace irr{
	namespace scene{
		class ISceneNode;
	};
};


namespace IrrAdapter{
	class Background{
	public:
		Background();
		void setResouceName(
			tString top, tString bottom=_T(""), tString left=_T(""), tString right=_T(""), tString front=_T(""), tString back=_T("")
		);
		void draw();
	private:
		std::array<tString, 6> m_Names;
		std::shared_ptr<irr::scene::ISceneNode> m_Node;
	};
};