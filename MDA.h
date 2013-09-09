#pragma once
#include "GLAS.hpp"

class IModel{
public:
	virtual ~IModel() = 0 {};
	virtual bool isValid() = 0;
};

class IDrawer{
public:
	virtual ~IDrawer() = 0 {};
	virtual bool isVisible() = 0;
	virtual void draw() = 0;
};

class IAgent{
public:
	virtual ~IAgent() = 0 {};
	virtual bool isAlive() = 0;
	virtual void step() = 0;
};



class Positionable{
public:
	virtual ~Positionable() = 0 {};
	virtual Glas::Vector3f getPosition() = 0;
};

class Rotatable{
public:
	virtual ~Rotatable() = 0 {};
	virtual Glas::Quaternion getAttitude() = 0;
};

class I3DAgent : public IAgent, public Positionable, public Rotatable{};