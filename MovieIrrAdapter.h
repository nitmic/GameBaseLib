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


namespace irr{
	namespace scene{
		class IMeshSceneNode;
	};
};

class murmuurVIDEO;

namespace IrrAdapter{
	class Movie{
	public:
		Movie();
		bool open(std::string name);
		bool refresh();
		void close();

	private:
		std::shared_ptr<irr::scene::IMeshSceneNode> m_pNode;
		std::shared_ptr<murmuurVIDEO> m_VideoPlayer;
		bool m_Playing;
	};
};