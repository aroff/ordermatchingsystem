#include "Order.h"

OMS::Order::Order()
{
}

OMS::Order::~Order()
{
}

bool OMS::Order::is_limit() const
{
	return price() > 0;
}

bool OMS::Order::all_or_none() const
{
	return false;
}

bool OMS::Order::imediate_or_cancel() const
{
	return false;
}
