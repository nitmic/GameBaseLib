#include "IrrAdapter.h"
#include "SpriteIrrAdapter.h"

#include <memory>
#include <sstream>

#include <FPSModerator.h>
#include <tString.h>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

struct IrrApp::Impl{
	bool fpsDisplay;
	std::shared_ptr<irr::IrrlichtDevice> device;
	std::function<bool(void)>       m_onFrameUpdate;    //!<	更新用関数ポインタ
	std::function<void(void)>       m_onFrameDraw;      //!<	描画用関数ポインタ
};


IrrApp::IrrApp(bool fpsDisplay){
	__impl__ = std::make_shared<Impl>();
	__impl__->fpsDisplay = fpsDisplay;
	__impl__->m_onFrameUpdate = [](){return true;};
	__impl__->m_onFrameDraw = [](){};
}

void IrrApp::setOnFrameUpdate(std::function<bool(void)> func){
	__impl__->m_onFrameUpdate = func;
}

void IrrApp::setOnFrameDraw(std::function<void(void)> func){
	__impl__->m_onFrameDraw = func;
}

bool IrrApp::Setup(){
	auto device_raw =irr::createDevice(
		IrrDefaultEngine, irr::core::dimension2d<irr::u32>(800, 600), 32,
		false, true, false, nullptr
	);
	if (!device_raw) return false;
	__impl__->device = std::shared_ptr<irr::IrrlichtDevice>(device_raw, IrrSafeRelease());
	
	accessVideoDriver()->setTextureCreationFlag(irr::video::ETCF_ALWAYS_32_BIT,true); 
	accessVideoDriver()->setTextureCreationFlag(irr::video::ETCF_ALWAYS_16_BIT,false); 
	accessVideoDriver()->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS,false);
	return true;
}

namespace{
	void visibleReset(irr::scene::ISceneNode * node){
		if(!node->isVisible()) return;
		node->setVisible(false);
		for(auto it=node->getChildren().begin(); it!=node->getChildren().end(); it++){
			visibleReset(*it);
		}
	}
};

void IrrApp::AppLoop(){
	auto fpsModerator = TUL::FPSModerator(60);
	auto lastFPS = -1;
	auto driver = accessVideoDriver();
	auto sm = accessSceneManager();
	auto env =  accessGUIEnvironment(); 

	while(__impl__->device->run()){
		//アクティブじゃなかったら処理しない
		if(!__impl__->device->isWindowActive()){
			//ビジーループを回避するために
			__impl__->device->yield(); 
		}else if(fpsModerator.step()){
			visibleReset(sm->getRootSceneNode());
			sm->getRootSceneNode()->setVisible(true);

			if(!__impl__->m_onFrameUpdate()) break;
			__impl__->m_onFrameDraw();
			driver->beginScene(true, true, irr::video::SColor(255,100,101,140));
			sm->drawAll();
			env->drawAll();
			IrrAdapter::DrawSprite();
			driver->endScene();
		}else{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}
		
		auto fps = driver->getFPS();
		if(lastFPS != fps && __impl__->fpsDisplay){
			toStringStream ostr;
			ostr << _T("fps: ") << fps;
			__impl__->device->setWindowCaption(ostr.str().c_str());
			lastFPS = fps;
		}
	}
}


irr::IrrlichtDevice * IrrApp::accessDevice(){
	return __impl__->device.get();
}
irr::video::IVideoDriver * IrrApp::accessVideoDriver(){
	return __impl__->device->getVideoDriver();
}
irr::scene::ISceneManager * IrrApp::accessSceneManager(){
	return __impl__->device->getSceneManager();
}
irr::gui::IGUIEnvironment * IrrApp::accessGUIEnvironment(){
	return __impl__->device->getGUIEnvironment();
}

#ifdef WIN32
HWND IrrApp::accessHWND(){
	if(IrrDefaultEngine==irr::video::EDT_OPENGL){
		return (HWND)accessVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd;
	}else if(IrrDefaultEngine==irr::video::EDT_DIRECT3D9){
		return (HWND)accessVideoDriver()->getExposedVideoData().D3D9.HWnd;
	}
}
#endif