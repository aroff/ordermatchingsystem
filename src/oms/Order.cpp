#include "Order.h"

OMS::Order::Order(const Trader &trader, Side side, Quantity quantity, Filling filling) :
		side_(side)
	,	quantity_(quantity)
	,	filling_(filling)
	,	trader_(trader)
	,	creation_(DateTime::now())
{
}

OMS::Order::~Order()
{
}

OMS::Side OMS::Order::side() const
{
	return side_;
}

OMS::Filling OMS::Order::filling() const
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

	if (stop_ == 0)
		return OrderError::InvalidStop;

	return OrderError::Ok;
}

OMS::Quantity OMS::Order::quantity() const
{
	return quantity_;
}

OMS::Trader OMS::Order::trader() const
{
	return trader_;
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

	if (limit_ == 0) {
		return OrderError::InvalidLimitPrice;
	}

	return OrderError::Ok;
}

OMS::OrderPtr OMS::MarketOrder::Create(const Trader &trader, Side side, Quantity quantity, Filling filling)
{
	return std::make_shared<OMS::MarketOrder>(MarketOrder(trader, side, quantity, filling));
}

OMS::MarketOrder::MarketOrder(const Trader &trader, Side side, Quantity quantity, Filling filling) :
	Order(trader, side, quantity, filling)
{
}

OMS::OrderError OMS::MarketOrder::validate() const
{
	return Order::validate();
}

OMS::OrderPtr OMS::LimitOrder::Create(const Trader &trader, Side side, Price limit, Quantity quantity, Filling filling)
{
	return std::move(std::make_shared<LimitOrder>(LimitOrder(trader, side, quantity, limit, filling)));
}

OMS::LimitOrder::LimitOrder(const Trader &trader, Side side, Quantity quantity, Price limit, Filling filling) :
		Order(trader, side, quantity, filling)
	,	limit_(limit)
{
}

OMS::OrderPtr OMS::StopLimitOrder::Create(const Trader &trader, Side side, Price limit, Price stop, Quantity quantity, Filling filling)
{
	return std::move(std::make_shared<StopLimitOrder>( StopLimitOrder(trader, side, quantity, stop, limit, filling)));
}

OMS::StopLimitOrder::StopLimitOrder(const Trader &trader, Side side, Quantity quantity, Price stop, Price limit, Filling filling) :
		LimitOrder(trader, side, quantity, limit, filling)
	,	stop_(stop)

{
}

OMS::OneCancelOtherOrder::OneCancelOtherOrder(const Trader &trader, Side side, Quantity quantity, Price limit, Price stop, Price stopLimit, Filling filling) :
		StopLimitOrder(trader, side, quantity, stop, limit, filling)
	,	stopLimit_(stopLimit)
{

}

OMS::OrderPtr OMS::OneCancelOtherOrder::Create(const Trader &trader, Side side, Price limit, Price stop, Price stopLimit, Quantity quantity, Filling filling)
{
	return std::move(std::make_shared<OneCancelOtherOrder>(OMS::OneCancelOtherOrder(trader, side, quantity, limit, stop, stopLimit, filling)));
}

OMS::OrderError OMS::OneCancelOtherOrder::validate() const
{
	return OrderError();
}
