#include "Debug.h"

#include <Singleton.hpp>
#include "IrrAdapter.h"
#include <sstream>

void Debug(tString str){
	GetSingleton<IrrApp>()->accessDevice()->setWindowCaption(str.c_str());
}