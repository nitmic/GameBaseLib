#include "WaitScene.h"

#include "SceneHandler.h"
#include <assert.h>



WaitScene::WaitScene(int wait) : timelimit(wait){
	assert(wait>0);
}

void WaitScene::step(
	SceneHandler * sceneStack
){
	if(timelimit>0){
		--timelimit;
		sceneStack->setNextScene(sceneStack->getCurrentScene());
	}
}