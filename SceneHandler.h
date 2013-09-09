#pragma once
#include <vector>
#include <map>

#include "IScene.h"
#include <tString.h>

class SceneHandler{
public:
	typedef std::vector<Scene> SceneStack;
	typedef SceneStack::iterator iterator;

	void goToNextScene();
	void setNextScene(Scene scene);
	Scene getCurrentScene();

	void loadSceneStack(tString name);
	void saveSceneStack(tString name);
	
	iterator getBegin();
	iterator getEnd();
	bool isNotHaveNextScene();
private:
	std::map<tString,SceneStack> m_PreservedScenes;
	Scene m_currentScene;
	SceneStack m_SceneStack;
};

