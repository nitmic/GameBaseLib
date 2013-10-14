#include "WaitScene.h"

#include "SceneHandler.h"
#include <assert.h>



WaitScene::WaitScene(int wait, std::function<void(int)> func) : timelimit(wait), wait(wait), func(func){
	assert(wait>0);
}

void WaitScene::step(
	SceneHandler * sceneStack
){
	if(timelimit>0){
		func(wait-timelimit);
		--timelimit;
		sceneStack->setNextScene(sceneStack->getCurrentScene());
	}
}