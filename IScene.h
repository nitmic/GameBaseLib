#pragma once
#include <memory>

class SceneHandler;

class IScene{
public:
	virtual ~IScene() = 0 {};
	virtual void step(
		SceneHandler * sceneStack
	) = 0;
	virtual void draw() = 0;
	virtual bool isTransparency() = 0;
};


typedef std::shared_ptr<IScene> Scene;
