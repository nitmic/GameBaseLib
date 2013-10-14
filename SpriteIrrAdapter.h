#pragma once
#include <memory>

#include <tString.h>
#include "GLAS.hpp"

namespace IrrAdapter{
	void DrawSprite();
};



namespace IrrAdapter{
	struct SpriteData;

	class Image;

	class Sprite{
	public:
		Sprite();
		~Sprite(){}
		void setPosition(Glas::Vector2i pos);
		void setResouceName(tString name);
		void setPriority(int degree);
		void setResouce(Image tex);
		void draw();
	private:
		std::shared_ptr<SpriteData> m_Object; 
		int m_Degree;
		tString m_name;
	};
};