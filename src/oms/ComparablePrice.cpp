#include "ComparablePrice.h"


OMS::ComparablePrice::ComparablePrice(OMS::Side side, OMS::Price price) :
		price_(price)
	,	side_(side)
{
}

bool OMS::ComparablePrice::match(Price price)
{
	if (side_ == OMS::Side::Buy) {
		return price_ >= price;
	}
	else
		return price_ <= price;
}

bool OMS::ComparablePrice::operator<(const ComparablePrice & rhs) const
{
	return *this < rhs.price_;
}

bool OMS::operator<(const ComparablePrice &lhs, Price rhs)
{
	return lhs.price() > rhs;
}
