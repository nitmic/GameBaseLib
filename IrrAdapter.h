#pragma once
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
#include <irrlicht.h>

const std::string IrrBlankXFile = "blank.x";
const irr::video::E_DRIVER_TYPE IrrDefaultEngine = irr::video::EDT_OPENGL;

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

#include <functional>
#include <Singleton.hpp>

class IrrApp : public Singleton<IrrApp>{
	friend Singleton<IrrApp>;
public:
	IrrApp();
	bool    Setup();
	void	setOnFrameUpdate(std::function<bool(void)> func);
	void	setOnFrameDraw(std::function<void(void)> func);
	void	AppLoop();

	irr::IrrlichtDevice * accessDevice();
	irr::video::IVideoDriver * accessVideoDriver();
	irr::scene::ISceneManager * accessSceneManager();

private:
	struct Impl;
	std::shared_ptr<Impl> __impl__;
};