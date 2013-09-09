#pragma once
#include <memory>

#include "GLAS.hpp"

class ICamera{
public:
	virtual void step() = 0;
	virtual void transform() = 0; // ‚±‚ê‚ÍAdapter‚É”C‚¹‚é
	
	virtual Glas::Quaternion getAttitude() const = 0;
	virtual Glas::Vector3f getPosition() const = 0;
};

/* –¢ŽÀ‘• */
class EditCamera : public ICamera{
public:
	void step();
	Glas::Quaternion getAttitude() const;
	Glas::Vector3f getPosition() const;
};

class I3DAgent;

class TPSCamera : public ICamera{
public:
	void step();
	Glas::Quaternion getAttitude() const;
	Glas::Vector3f getPosition() const;
	Glas::Vector3f getLookAt() const;
	float getDistance() const;

	void setTarget(const std::shared_ptr<I3DAgent> & target);
	void setDistance(float d);
	void setAttitude(Glas::Quaternion & q);

	void rotate(TUL::Degree deg, Glas::Vector3f v=Glas::Vector3f(0,1,0));
	void normalize();

	TPSCamera();
	TPSCamera(std::shared_ptr<I3DAgent> target);
private:
	struct Impl;
	std::shared_ptr<Impl> __impl__;
};

