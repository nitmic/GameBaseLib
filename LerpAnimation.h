#pragma once
#include <functional>
#include <assert.h>

class LerpAnimation{
public:
	LerpAnimation() : currentFrame(-1), numOfFrames(-1){};
	LerpAnimation(int numOfFrames, double to, double from, std::function<double(void)> callback)
		: currentFrame(0),numOfFrames(numOfFrames), to(to), from(from), callback(callback){
		assert(numOfFrames>0);
	};
	double next(){
		assert(currentFrame!=-1);
		
		++currentFrame;
		if(currentFrame==numOfFrames){
			++currentFrame;
			return callback();
		}
		return from + (to-from)*((double)currentFrame/(double)numOfFrames);
	}


	bool isAnimating(){
		return currentFrame<numOfFrames;
	}
private:
	int numOfFrames;
	int currentFrame;
	double from;
	double to;
	std::function<double(void)> callback;
};