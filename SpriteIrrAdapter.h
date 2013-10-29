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
	*  �摜�I�u�W�F�N�g�̕`��B�@�摜�I�u�W�F�N�g�̐ӔC������
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