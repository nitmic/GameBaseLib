#pragma once
#include "IScene.h"

class WaitScene : public IScene{
public:
	WaitScene(int wait);

	bool isTimeup(){return timelimit==0;}
	void step(
		SceneHandler * sceneStack
	);
	void draw(){};
	bool isTransparency(){return true;};
private:
	int timelimit;
};