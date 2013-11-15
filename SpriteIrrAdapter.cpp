#include "SpriteIrrAdapter.h"
#include <stack>
#include <array>
#include <Singleton.hpp>
#include <assert.h>
#include <functional>

#include "IrrAdapter.h"
#include "ImageIrrAdapter.h"

namespace IrrAdapter{
	struct SpriteData{
		SpriteData() : tex(nullptr){}
		std::shared_ptr<Image> tex;
		irr::core::vector2di pos;
	};
};

namespace{
	std::array<std::stack<std::shared_ptr<IrrAdapter::SpriteData>>,10> g_Sprites;
};

namespace IrrAdapter{
	void DrawSprite(){
		auto pos = irr::core::position2di(0, 0);
		auto driver = GetSingleton<IrrApp>()->accessVideoDriver();
		for(auto it=g_Sprites.begin();it!=g_Sprites.end();++it){
			while(!it->empty()){
				auto image = it->top();
				if(image->tex->getRaw()){
					driver->draw2DImage(
						image->tex->getRaw(), image->pos, irr::core::recti(pos, image->tex->getRaw()->getSize()),
						0, irr::video::SColor(255,255,255,255), true
					);
				}
				it->pop();
			}
		}
	}


	Sprite::Sprite() : m_Degree(0){
		m_Object = std::make_shared<SpriteData>();
	}

	void Sprite::draw(){
		if(m_Object->tex==nullptr) return;
		g_Sprites[m_Degree].push(m_Object);
	}

	void Sprite::setPosition(Glas::Vector2i pos){
		m_Object->pos = pos;
	}
	void Sprite::setResouceName(TUL::tString name){
		if(m_Object->tex && m_Object->tex->getName()==name) return;

		m_Object->tex = std::make_shared<Image>(name.c_str());
	}
	void Sprite::setResouce(std::shared_ptr<Image> tex){
		if(m_Object->tex && m_Object->tex->getRaw()==tex->getRaw()) return;
		m_Object->tex = tex;
	}
	void Sprite::setPriority(int degree){
		assert(degree>=0);
		m_Degree = degree;
	}
};
