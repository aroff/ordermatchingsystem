#pragma once
#include "types.h"

namespace OMS {

	enum OrderError {
		Ok,
		InvalidQuantity,
		LimitMustBeHigherThanStop,
		LimitMustBeLowerThanStop,
		InvalidLimitPrice,
		InvalidStop
		
	};

	class Order
	{
	public:

		enum Type { Market, Limit, StopLimit, OCO};

		enum Filling { 
			PartialFill,
			AllOrNothing, //only confirm trade when completelly filled. they have lower priority 
			ImmediateOrCancel  // requires all or part of the order to be executed immediately, and any unfilled parts of the order are canceled
		};

		enum Side { Buy, Sell };

		/// is this a limit order?
		bool is_limit() const;

		virtual Type type() const;
		virtual Side side() const;
		virtual Filling filling() const;
		virtual OrderError validate() const;

		virtual Quantity quantity() const;

		virtual ~Order();
	
	protected:
		Order(Type type, Side side, Quantity quantity, Filling filling);
		Order();

		Side side_;
		Quantity quantity_;
		Type type_;
		Filling filling_;
	};

	class MarketOrder : public Order {
	public:
		MarketOrder(Side side, Quantity quantity, Filling filling);
		virtual OrderError validate() const;

		static MarketOrder Create(Side side, Quantity quantity, Filling filling = Filling::PartialFill);

	protected:
	};

	class LimitOrder : public Order {
	public:
		LimitOrder(Side side, Quantity quantity, Price limit, Filling filling);
		static LimitOrder Create(Side type, Price limit, Quantity quantity, Filling filling = Filling::PartialFill);
		
		virtual Price limit() const; // Limit price
		virtual OrderError validate() const;

	protected:
		Price limit_;
	};

	class StopLimitOrder : public LimitOrder {
	public:
		StopLimitOrder(Side side, Quantity quantity, Price stop, Price limit, Filling filling);
		static StopLimitOrder Create(Side type, Price limit, Price stop, Quantity quantity, Filling filling = Filling::PartialFill);

		/// Price of this order
		virtual Price stop() const;
		virtual OrderError validate() const;

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
		OneCancelOtherOrder(Side side, Quantity quantity, Price limit, Price stop, Price stopLimit, Filling filling);
		static OneCancelOtherOrder Create(Side type, Price limit, Price stop, Price stopLimit, Quantity quantity, Filling filling = Filling::PartialFill);

		/// Price of this order
		virtual OrderError validate() const;

	protected:

		Price stopLimit_;
	};


}
