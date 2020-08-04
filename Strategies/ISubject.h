#pragma once

#include "IObserver.h"

class ISubject {
public:
	virtual bool RegisterObserver(IObserver *pObserver) = 0;
	virtual void Notify() = 0;
};