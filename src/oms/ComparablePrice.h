#pragma once
#include "types.h"
#include "order.h"

namespace OMS {

	/*
	This class implements a "smart price comparison", by taking into account the transaction side (buy/sell) and check for matching. 
	*/
	class ComparablePrice
	{
	public:
		ComparablePrice(OMS::Order::Side side, OMS::Price price);
		Price price() const { return price_; }

		bool match(Price price); 

		/*bool operator <(Price rhs) const;
		bool operator ==(Price rhs) const;
		bool operator !=(Price rhs) const;
		bool operator > (Price rhs) const;
		bool operator <=(Price rhs) const;
		bool operator >=(Price rhs) const;*/
		bool operator <(const ComparablePrice & rhs) const;
		//bool operator ==(const ComparablePrice & rhs) const;
		//bool operator !=(const ComparablePrice & rhs) const;



	protected:
		OMS::Price price_;
		OMS::Order::Side side_;
	};

	bool operator <(const ComparablePrice &lhs, Price rhs);
	

}