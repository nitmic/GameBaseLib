#include "SceneHandler.h"

SceneHandler::iterator SceneHandler::getBegin(){
	return m_SceneStack.begin();
}


SceneHandler::iterator SceneHandler::getEnd(){
	return m_SceneStack.end();
}

bool SceneHandler::isNotHaveNextScene(){
	return m_SceneStack.empty();
}

void SceneHandler::goToNextScene(){
	m_currentScene = m_SceneStack.back();
	m_SceneStack.pop_back();
}

void SceneHandler::setNextScene(Scene scene){
	m_SceneStack.push_back(scene);
}

Scene SceneHandler::getCurrentScene(){
	return m_currentScene;
}

void SceneHandler::loadSceneStack(tString name){
	m_SceneStack = m_PreservedScenes[name];
}

void SceneHandler::saveSceneStack(tString name){
	m_PreservedScenes[name] = m_SceneStack;
}