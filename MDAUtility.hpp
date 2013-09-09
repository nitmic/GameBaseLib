#pragma once
#include <SmartPointerUtility.hpp>
#include "MDA.h"


/**
*@brief	IModelが有効かどうか. 有効なら関数を呼ぶ.
*@return bool
*@param p    : shared_ptr or weak_ptr, or they are wrapped by vector.
        func : When IModel::isValid return true, it is called.
*/
struct isValidObject{
	template<class T>
	bool operator()(T v1){
		return isValid(v1);
	}
	
	template<class T1, class T2>
	bool operator()(T1 v1, T2 v2){
		return isValid(v1, v2);
	}

	template<class T1, class T2>
	void operator()(std::vector<T1> & v1, T2 v2){
		return isValid(v1, v2);
	}
};

template<class T>
bool isValid(T & p){
	return isTrue(p, &IModel::isValid);
}

template<class T>
bool isValid(T & p, std::function<void(typename toShared<T>::type &)> func){
	return isTrue(p, func, &IModel::isValid);
}

//std::vector用
template<class T>
void isValid(std::vector<T> & v, std::function<void(typename toShared<T>::type &)> func){
	isTrue(v, func, &IModel::isValid);
}


/**
*@brief	IModelが有効かどうか. 有効なら関数を呼び, falseならeraseする.
*@return void
*@param v    : std::vector<shared_ptr or weak_ptr>
        func : When IModel::isValid return true, it is called.
*/
struct isValidOrEraseObject{
	template<class T>
	void operator()(T v){
		return isValidOrErase(v);
	}

	template<class T1, class T2>
	void operator()(T1 v1, T2 v2){
		return isValidOrErase(v1, v2);
	}
};


template<class T>
void isValidOrErase(std::vector<T> & v){
	isTrueOrErase(v, doNothing<typename toShared<T>::type>, &IModel::isValid);
}
template<class T>
void isValidOrErase(std::vector<T> & v, std::function<void(typename toShared<T>::type &)> func){
	isTrueOrErase(v, func, &IModel::isValid);
}


/**
*@brief	IDrawerが有効かどうか. 有効なら関数を呼ぶ.
*@return bool
*@param p    : shared_ptr or weak_ptr, or they are wrapped by vector.
        func : When IDrawer::isVisible return true, it is called.

*/
struct isVisibleObject{
	template<class T>
	bool operator()(T v1){
		return isVisible(v1);
	}
	
	template<class T1, class T2>
	bool operator()(T1 v1, T2 v2){
		return isVisible(v1, v2);
	}

	template<class T1, class T2>
	void operator()(std::vector<T1> & v1, T2 v2){
		return isVisible(v1, v2);
	}
};

template<class T>
bool isVisible(T & p){
	return isTrue(p, &IDrawer::isVisible);
}
template<class T>
bool isVisible(T & p, std::function<void(typename toShared<T>::type &)> func){
	return isTrue(p, func, &IDrawer::isVisible);
}

//std::vector用
template<class T>
void isVisible(std::vector<T> & v, std::function<void(typename toShared<T>::type &)> func){
	isTrue(v, func, &IDrawer::isVisible);
}

/**
*@brief	IDrawerが有効かどうか. 有効なら関数を呼び, falseならeraseする.
*@return void
*@param v    : std::vector<shared_ptr or weak_ptr>
        func : When IDrawer::isVisible return true, it is called.
*/
struct isVisibleOrEraseObject{
	template<class T>
	void operator()(T v){
		return isVisibleOrErase(v);
	}

	template<class T1, class T2>
	void operator()(T1 v1, T2 v2){
		return isVisibleOrErase(v1, v2);
	}
};

template<class T>
void isVisibleOrErase(std::vector<T> & v){
	isTrueOrErase(v, doNothing<typename toShared<T>::type>, &IDrawer::isVisible);
}
template<class T>
void isVisibleOrErase(std::vector<T> & v, std::function<void(typename toShared<T>::type &)> func){
	isTrueOrErase(v, func, &IDrawer::isVisible);
}



/**
*@brief	IAgentが有効かどうか. 有効なら関数を呼ぶ.
*@return bool
*@param p    : shared_ptr or weak_ptr, or they are wrapped by vector.
        func : When IAgent::isAlive return true, it is called.

*/
struct isAliveObject{
	template<class T>
	bool operator()(T v1){
		return isAlive(v1);
	}
	
	template<class T1, class T2>
	bool operator()(T1 v1, T2 v2){
		return isAlive(v1, v2);
	}

	template<class T1, class T2>
	void operator()(std::vector<T1> & v1, T2 v2){
		return isAlive(v1, v2);
	}
};

template<class T>
bool isAlive(T & p){
	return isTrue(p, &IAgent::isAlive);
}
template<class T>
bool isAlive(T & p, std::function<void(typename toShared<T>::type &)> func){
	return isTrue(p, func, &IAgent::isAlive);
}

//std::vector用
template<class T>
void isAlive(std::vector<T> & v, std::function<void(typename toShared<T>::type &)> func){
	isTrue(v, func, &IAgent::isAlive);
}

/**
*@brief	IAgentが有効かどうか. 有効なら関数を呼び, falseならeraseする.
*@return void
*@param v    : std::vector<shared_ptr or weak_ptr>
        func : When IAgent::isAlive return true, it is called.
*/
struct isAliveOrEraseObject{
	template<class T>
	void operator()(T v){
		return isAliveOrErase(v);
	}

	template<class T1, class T2>
	void operator()(T1 v1, T2 v2){
		return isAliveOrErase(v1, v2);
	}
};

template<class T>
void isAliveOrErase(std::vector<T> & v){
	isTrueOrErase(v, doNothing<typename toShared<T>::type>, &IAgent::isAlive);
}
template<class T>
void isAliveOrErase(std::vector<T> & v, std::function<void(typename toShared<T>::type &)> func){
	isTrueOrErase(v, func, &IAgent::isAlive);
}

/**
*@brief IAgent::step用ラッパー関数
*/
struct stepping_imp{
template<class T>
	void operator()(T & p){
		p->step();
	}
};

template<class T>
void stepping(T & p){
	isAlive(p, stepping_imp());
}

template<class T>
void stepping(std::vector<T> & v){
	isAliveOrErase(v, stepping_imp());
}


/**
*@brief IDrawer::draw用ラッパー関数
*/
struct drawing_imp{
	template<class T>
	void operator()(T & p){
		p->draw();
	}
};

template<class T>
void drawing(T & p){
	isVisible(p, drawing_imp());
}

template<class T>
void drawing(std::vector<T> & v){
	isVisible(v, drawing_imp());
}