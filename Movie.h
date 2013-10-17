#pragma once
#include <string>
#include <memory>

extern "C" {
   #include <string.h>
   #include <libavcodec/avcodec.h>
   #include <libavformat/avformat.h>
   #include <libswscale/swscale.h>
#ifdef SOUND_OPENAL
   #include <signal.h>
   #include <AL/al.h>
   #include <AL/alc.h>
   #include <AL/alut.h>
#endif
}


class Movie{
public:
	enum State{Closed, Playing, Paused, Stopped };

	Movie() : m_Width(-1), m_Height(-1){};
	Movie(int width, int height) : m_Width(width), m_Height(height){init();}
	~Movie(){ if(m_State != Closed) close();}
	
	void init(int width, int height);
	bool open(std::string fileName);
	bool refresh();
	void close();
	void setScreen(int width, int height);
	
	bool seek(int tsms);
	bool seekByFrame(int frame);
private:
	struct Impl;
	std::shared_ptr<Impl> __impl__;

	void init();
	void setScreen();
	
	bool m_NeedResize;
	int m_Width;
	int m_Height;
	State m_State;
};