#pragma once
#include <functional>
#include "IScene.h"

class WaitScene : public IScene{
public:
	WaitScene(int wait, std::function<void(int)> func = [](int){});

	bool isTimeup(){return timelimit==0;}
	void step(
		SceneHandler * sceneStack
	);
	void draw(){};
	bool isTransparency(){return true;};
private:
	std::function<void(int)> func;
	int wait;
	int timelimit;
};