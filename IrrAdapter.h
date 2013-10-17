#pragma once
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>

const irr::io::path IrrBlankXFile = "./Mesh/blank.x";
const irr::video::E_DRIVER_TYPE IrrDefaultEngine = irr::video::EDT_DIRECT3D9;


struct IrrSafeRelease{
	template<class T>
	void operator()(T * p){
		p->drop();
	}
};

struct IrrSafeRemove{
	template<class T>
	void operator()(T * p){
		p->remove();
	}
};



#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

#include <functional>
#include <Singleton.hpp>

class IrrApp : public Singleton<IrrApp>{
	friend Singleton<IrrApp>;
public:
	IrrApp(bool fpsDisplay=true);
	bool    Setup(int width, int height);
	void	setOnFrameUpdate(std::function<bool(void)> func);
	void	setOnFrameDraw(std::function<void(void)> func);
	void	AppLoop();

	irr::IrrlichtDevice * accessDevice();
	irr::video::IVideoDriver * accessVideoDriver();
	irr::scene::ISceneManager * accessSceneManager();
	irr::gui::IGUIEnvironment * accessGUIEnvironment();
	
	#ifdef WIN32
	HWND accessHWND();
	#endif

private:
	struct Impl;
	std::shared_ptr<Impl> __impl__;
};