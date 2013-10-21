#pragma once
#include <memory>
#include <string>

#include "IrrAdapter.h"


#ifdef _MSC_VER
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#endif

namespace TUL{
	class MovieDecoder;
};

namespace irr{
	namespace video{
		class IImage;
	};
};

namespace IrrAdapter{
	class Image;
	/*
	*@class IrrAdapter::Movie
	*  画像オブジェクトの生成。　画像オブジェクトおよびデコーダ本体の責任を持つ
	*/
	class Movie{
	public:
		Movie() : m_Output(nullptr){}
		Movie(std::shared_ptr<Image> & output, int w=800, int h=600);
		void changeDecodeSize(int w, int h);
		bool open(std::string name);
		bool refresh();
		void close();
	private:
		int m_W, m_H;
		unsigned char * m_Raw;
		irr::video::IImage * m_Image;
		std::shared_ptr<Image> m_Output;
		std::shared_ptr<TUL::MovieDecoder> m_Decoder;
	};
};