#include "IrrAdapter.h"

#include <memory>
#include <sstream>

#include <FPSModerator.h>
#include <tString.h>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

struct IrrApp::Impl{
	std::shared_ptr<irr::IrrlichtDevice> device;
	std::function<bool(void)>       m_onFrameUpdate;    //!<	更新用関数ポインタ
	std::function<void(void)>       m_onFrameDraw;      //!<	描画用関数ポインタ
};


IrrApp::IrrApp(){
	__impl__ = std::make_shared<Impl>();
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
	return true;
}

namespace{
	void visibleReset(irr::scene::ISceneNode * node){
		if(!node->isVisible()) return;
		node->setVisible(false);
		for(auto it=node->getChildren().begin();it!=node->getChildren().end();it++){
			visibleReset(*it);
		}
	}
};

void IrrApp::AppLoop(){
	auto fpsModerator = TUL::FPSModerator(60);
	auto lastFPS = -1;
	auto driver = accessVideoDriver();
	auto sm = accessSceneManager();
	auto env =  __impl__->device->getGUIEnvironment(); 

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
			driver->endScene();
		}else{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}
		
		auto fps = driver->getFPS();
		if(lastFPS != fps){
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