#include "Camera.h"
#include "MDA.h"
#include <AngleUnit.h>

struct DefaultCamera::Impl{
	Impl(){};
	Glas::Quaternion attitude;
	Glas::Vector3f position;
	unsigned int            cameraIndex;
};

DefaultCamera::DefaultCamera(){
	__impl__ = std::make_shared<Impl>();
}

Glas::Quaternion DefaultCamera::getAttitude() const{
	return __impl__->attitude;
}

Glas::Vector3f DefaultCamera::getPosition() const{
	return __impl__->position;
}

Glas::Vector3f DefaultCamera::getLookAt() const{
	return getPosition() + getAttitude() * Glas::Vector3f(0,0,1);
}

void DefaultCamera::setAttitude(Glas::Quaternion & q){
	__impl__->attitude = q;
}

void DefaultCamera::setPosition(Glas::Vector3f & p){
	__impl__->position = p;
}


struct TPSCamera::Impl{
	Impl() : distance(4) {};
	std::shared_ptr<I3DAgent> target;
	Glas::Quaternion attitude;
	float                   distance;
	unsigned int            cameraIndex;
};

TPSCamera::TPSCamera(){
	__impl__ = std::make_shared<Impl>();
};

TPSCamera::TPSCamera(std::shared_ptr<I3DAgent> target){
	__impl__ = std::make_shared<Impl>();
	setTarget(target);
};

void TPSCamera::setTarget(const std::shared_ptr<I3DAgent> & target){
	__impl__->target = target;
	normalize();
}


Glas::Vector3f TPSCamera::getPosition() const{
	auto direction = __impl__->attitude * Glas::Vector3f(0,0,1);
	return getLookAt() - direction.normalize() * __impl__->distance;
}

Glas::Vector3f TPSCamera::getLookAt() const{
	return __impl__->target->getPosition();
}

float TPSCamera::getDistance() const{
	return __impl__->distance;
}

Glas::Quaternion TPSCamera::getAttitude() const{
	return __impl__->attitude;
}

void TPSCamera::setAttitude(Glas::Quaternion & q){
	__impl__->attitude = q;
	__impl__->attitude.normalize();
}

void TPSCamera::setDistance(float d){
	if(d<2.5) return;
	__impl__->distance = d;
}

void TPSCamera::normalize(){
	setAttitude(__impl__->target->getAttitude());
}

void TPSCamera::step(){}
void TPSCamera::rotate(TUL::Degree deg, Glas::Vector3f v){
	Glas::Quaternion attitude;
	attitude.fromAngleAxis(deg, v.normalize());
	attitude.normalize();
	setAttitude(__impl__->attitude * attitude);
}