#include "Order.h"

OMS::Order::Order(Type type, Side side, Quantity quantity, Filling filling) :
		type_(type)
	,	side_(side)
	,	quantity_(quantity)
	,	filling_(filling)
{
}

OMS::Order::Order()
{
}

OMS::Order::~Order()
{
}

bool OMS::Order::is_limit() const
{
	return type_ == Type::Limit || type_ == Type::StopLimit;
}

OMS::Order::Type OMS::Order::type() const
{
	return type_;
}

OMS::Order::Side OMS::Order::side() const
{
	return side_;
}

OMS::Order::Filling OMS::Order::filling() const
{
	return filling_;
}

OMS::OrderError OMS::Order::validate() const
{
	if (quantity_ <= 0)
		return OrderError::InvalidQuantity;

	return OrderError::Ok;
}

OMS::Price OMS::StopLimitOrder::stop() const
{
	return stop_;
}

OMS::OrderError OMS::StopLimitOrder::validate() const
{
	auto err = LimitOrder::validate();
	if(err != OrderError::Ok)
		return err;
	
	if (side_ == Side::Buy) {
		if (limit_ < stop_) {
			return OrderError::LimitMustBeHigherThanStop;
		}
	}
	else { // Sell order
		if (limit_ > stop_) {
			return OrderError::LimitMustBeLowerThanStop;
		}
	}

	if (stop_ <= 0)
		return OrderError::InvalidStop;

	return OrderError::Ok;
}

OMS::Quantity OMS::Order::quantity() const
{
	return quantity_;
}

OMS::Price OMS::LimitOrder::limit() const
{
	return limit_;
}

OMS::OrderError  OMS::LimitOrder::validate() const
{
	auto err = Order::validate();
	if (err != OrderError::Ok)
		return err;

	if (limit_ <= 0) {
		return OrderError::InvalidLimitPrice;
	}

	return OrderError::Ok;
}


OMS::MarketOrder OMS::MarketOrder::Create(Side side, Quantity quantity, Filling filling)
{
	return MarketOrder(side, quantity, filling);
}

OMS::MarketOrder::MarketOrder(Side side, Quantity quantity, Filling filling) :
	Order(Order::Type::Market, side, quantity, filling)
{
}

OMS::OrderError OMS::MarketOrder::validate() const
{
	return Order::validate();
}

OMS::LimitOrder OMS::LimitOrder::Create(Side side, Price limit, Quantity quantity, Filling filling)
{
	return LimitOrder(side, quantity, limit, filling);
}

OMS::LimitOrder::LimitOrder(Side side, Quantity quantity, Price limit, Filling filling) :
		Order(Order::Type::Limit, side, quantity, filling)
	,	limit_(limit)
{
}

OMS::StopLimitOrder OMS::StopLimitOrder::Create(Side side, Price limit, Price stop, Quantity quantity, Filling filling)
{
	return StopLimitOrder(side, quantity, stop, limit, filling);
}

OMS::StopLimitOrder::StopLimitOrder(Side side, Quantity quantity, Price stop, Price limit, Filling filling) :
		LimitOrder(side, quantity, limit, filling)
	,	stop_(stop)

{
}

OMS::OneCancelOtherOrder::OneCancelOtherOrder(Side side, Quantity quantity, Price limit, Price stop, Price stopLimit, Filling filling) :
		StopLimitOrder(side, quantity, stop, limit, filling)
	, stopLimit_(stopLimit)
{

}

OMS::OneCancelOtherOrder OMS::OneCancelOtherOrder::Create(Side side, Price limit, Price stop, Price stopLimit, Quantity quantity, Filling filling)
{
	return OMS::OneCancelOtherOrder(side, quantity, limit, stop, stopLimit, filling);
}

OMS::OrderError OMS::OneCancelOtherOrder::validate() const
{
	return OrderError();
}
