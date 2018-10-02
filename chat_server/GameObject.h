#pragma once
#include "Incl.h"

class GameObject
{
public:
	GameObject() {}

    virtual ~GameObject() {}

	virtual void update() = 0;
};

