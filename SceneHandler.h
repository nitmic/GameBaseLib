#pragma once
#include <deque>
#include <vector>
#include <array>

#include "IScene.h"
#include <tString.hpp>

class SceneHandler{
public:
	typedef std::deque<Scene> SceneStack;
	typedef SceneStack::iterator iterator;

	void goToNextScene();
	void setNextScene(Scene scene);
	Scene getCurrentScene();

	void loadSceneStack(int name);
	void saveSceneStack(int name);
	
	iterator getBegin();
	iterator getEnd();
	bool isNotHaveNextScene();
private:
	std::array<SceneStack,100> m_PreservedScenes;
	Scene m_currentScene;
	SceneStack m_SceneStack;
};

