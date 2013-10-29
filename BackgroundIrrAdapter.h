#pragma once
#include <memory>
#include <array>
#include <tString.hpp>


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
			TUL::tString top, TUL::tString bottom=_T(""), TUL::tString left=_T(""), TUL::tString right=_T(""), TUL::tString front=_T(""), TUL::tString back=_T("")
		);
		void draw();
	private:
		std::array<TUL::tString, 6> m_Names;
		std::shared_ptr<irr::scene::ISceneNode> m_Node;
	};
};