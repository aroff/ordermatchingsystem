#pragma once
#include "IObserver.h"
#include <string>

class AbstractStrategy : public IObserver
{
public:
	virtual void OnNotify() {
		//sleep(strategyTimeOut_ * 1000);
	}

private:
	std::string strategyName_;
	int strategyTimeOut_;

};

