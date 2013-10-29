#pragma once
#include <memory>

#include <tString.hpp>
#include "GLAS.hpp"

namespace IrrAdapter{
	void DrawSprite();
};



namespace IrrAdapter{
	struct SpriteData;

	class Image;
	
	/*
	*@class IrrAdapter::Sprite
	*  画像オブジェクトの描画。　画像オブジェクトの責任を持つ
	*/
	class Sprite{
	public:
		Sprite();
		void setPosition(Glas::Vector2i pos);
		void setResouceName(TUL::tString name);
		void setPriority(int degree);
		void setResouce(std::shared_ptr<Image> tex);
		void draw();
	private:
		std::shared_ptr<SpriteData> m_Object;
		int m_Degree;
	};
};