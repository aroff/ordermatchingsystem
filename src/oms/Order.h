#pragma once
#include "types.h"
#include "trader.h"
#include <memory>

namespace OMS {

	enum class Schedule {
		Day, //These orders expire at the end of the trading day.
		GTC, //(Good till canceled). This order is valid until it is cancelled. To make sure they don’t stay at the market indefinitely, a special limitation is imposed, which is usually from 30 to 90 days.
		FOK, // (Fill Or Kill). The order must be fully filled or immediately cancelled.
		IOC,// (Immediate Or Cancel). The order is immediately executed or cancelled by the exchange. Unlike FOK, this type allows for partial fulfillment.
		GTD,// (Good-til-Date/Time).
		GAT, // (Good-after-Time/Date).
	};

	enum class Filling {
		PartialFill,
		AllOrNothing, //only confirm trade when completelly filled. they have lower priority 
		ImmediateOrCancel  // requires all or part of the order to be executed immediately, and any unfilled parts of the order are canceled
	};

	enum class Side { Buy, Sell };


	enum class OrderError {
		Ok,
		InvalidQuantity,
		LimitMustBeHigherThanStop,
		LimitMustBeLowerThanStop,
		InvalidLimitPrice,
		InvalidStop
		
	};

	//https://www.vonmo.com/en/blog/vonmo-trade-experiment-orders-types-and-processing-features/
	class Order
	{
	public:

		//virtual Type type() const;
		virtual Side side() const;
		virtual Filling filling() const;
		virtual OrderError validate() const;

		virtual Quantity quantity() const;

		virtual Trader trader() const;

		virtual ~Order();
	
	protected:
		Order(const Trader &trader, Side side, Quantity quantity, Filling filling);
		//Order();

		Side side_;
		Quantity quantity_;
		//Type type_;
		Filling filling_;
		const Trader &trader_;
	};


	typedef std::shared_ptr<Order> OrderPtr;

	class MarketOrder : public Order {
	public:
		MarketOrder(const Trader &trader, Side side, Quantity quantity, Filling filling);
		virtual OrderError validate() const override;

		static OrderPtr Create(const Trader &trader, Side side, Quantity quantity, Filling filling = Filling::PartialFill);

	protected:
	};

	class LimitOrder : public Order {
	public:
		LimitOrder(const Trader &trader, Side side, Quantity quantity, Price limit, Filling filling);
		static OrderPtr Create(const Trader &trader, Side type, Price limit, Quantity quantity, Filling filling = Filling::PartialFill);

		virtual Price limit() const; // Limit price
		virtual OrderError validate() const;

	protected:
		Price limit_;
	};

	class StopLimitOrder : public LimitOrder {
	public:
		StopLimitOrder(const Trader &trader, Side side, Quantity quantity, Price stop, Price limit, Filling filling);
		static OrderPtr Create(const Trader &trader, Side type, Price limit, Price stop, Quantity quantity, Filling filling = Filling::PartialFill);

		/// Price of this order
		virtual Price stop() const;
		virtual OrderError validate() const override;

	protected:
		Price stop_;
	};

	/*
	The one cancels other order option allows you to place a pair of orders stipulating that if one order is executed fully or partially,
	then the other is automatically canceled. An OCO order combines a stop order with a limit order. This option allows you to place both
	take profit and stop loss targets for your position (only for limit orders).
	*/

	class OneCancelOtherOrder : public StopLimitOrder {
	public:
		OneCancelOtherOrder(const Trader &trader, Side side, Quantity quantity, Price limit, Price stop, Price stopLimit, Filling filling);
		static OrderPtr Create(const Trader &trader, Side type, Price limit, Price stop, Price stopLimit, Quantity quantity, Filling filling = Filling::PartialFill);

		/// Price of this order
		virtual OrderError validate() const override;

	protected:

		Price stopLimit_;
	};


}
