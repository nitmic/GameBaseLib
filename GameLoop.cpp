#include "GameLoop.h"
#include "SceneHandler.h"

namespace{
	void drawUnderScene(SceneHandler::iterator it, const SceneHandler::iterator & end){
		if(it==end) return;
		if((*it)->isTransparency()) drawUnderScene(it+1, end);
		(*it)->draw();
	}
};

struct GameLoop::Impl{
	SceneHandler sceneHandler;
};


GameLoop::GameLoop(Scene startScene){
	__impl__ = std::make_shared<Impl>();
	__impl__->sceneHandler.setNextScene(startScene);
}

bool GameLoop::update(){
	if(__impl__->sceneHandler.isNotHaveNextScene()) return false;
	__impl__->sceneHandler.goToNextScene();
	__impl__->sceneHandler.getCurrentScene()->step(&__impl__->sceneHandler);
	return true;
}

void GameLoop::draw(){
	if(__impl__->sceneHandler.getCurrentScene()->isTransparency()){
		drawUnderScene(
			__impl__->sceneHandler.getBegin(),
			__impl__->sceneHandler.getEnd()
		);
	}
	__impl__->sceneHandler.getCurrentScene()->draw();
}