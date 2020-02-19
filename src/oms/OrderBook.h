#pragma once
#include "Asset.h"
#include "Order.h"
#include "OrderTracker.h"
#include "ComparablePrice.h"

#include <map>
#include <list>
#include <mutex>

namespace OMS {

	typedef std::multimap<ComparablePrice, OrderTracker> OrderTrackerMap;
	typedef std::list<typename OrderTrackerMap::iterator> DeferredMatches;


	// This class represents the limit (maker) order book for a given asset
	class OrderBook
	{
	public:


		// Construct the Order book for Asset
		OrderBook(const Asset &asset, const Asset &baseAsset);
		virtual ~OrderBook();

		const Asset &asset() const;
		const Asset &baseAsset() const;

		// Order management methods
		virtual bool add(OrderPtr order/*, const OrderConditions orderConditions = 0*/);

		virtual bool cancel(OrderPtr order);

		virtual void replace(OrderPtr order, Quantity deltaQuantity, Price newPrice);

		// set current market price for the Asset
		virtual void set_price(Price price);

		virtual Price get_price() const;

		void print() const;
	protected:
		/// Events interfaces
		virtual void OnAccept(const Order &order);
		virtual void OnReject(const Order &order, OrderError err);
		virtual void OnFill(const Order &order, const Order &matchedOrder, Quantity filledQuantity, Cost fillCost, bool inboundOrderFilled, bool matchedOrderFilled);
		virtual void OnCancel(const Order &order);
		virtual void OnCancelReject(const Order &order, std::string reason);
		virtual void OnReplace(const Order &order, Quantity currentQuantity, Quantity newQuantity, Price newPrice);
		virtual void OnReplaceReject(const Order &order, std::string reason);
		virtual void OnTrade(const OrderBook &order, Quantity quantity, Cost cost);
		///

		virtual void OnOrderBookChange();

		// 

		//void check_stop_orders(bool side, Price price, )

	protected:

		void add_limit_order(OrderPtr order) noexcept;
		void add_market_order(OrderPtr order) noexcept;
		void add_stoplimit_order(OrderPtr order) noexcept;

		void try_match_market(OrderTracker &order) noexcept;
		void try_match_limit(OrderTracker &order) noexcept;
		void try_match_stoplimit(OrderTracker &order) noexcept;

		// trade order1 against order2
		Quantity do_trade(OrderTracker &income, OrderTracker &order, Quantity maxQuantity);

		// Matching
		virtual bool match_order(OrderTracker &incomeOrder, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons) noexcept;

		// matching of a regular order
		bool match_standard_order(OrderTracker &incomeOrder, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons) noexcept;


		// matching of an all or nothing order
		bool match_aon_order(OrderTracker &incomeOrder, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons) noexcept;

		const Asset &asset_;
		const Asset &baseAsset_;
		Price price_{ 0 };  // Market Price

		OrderTrackerMap Bids_;
		OrderTrackerMap Asks_;
		
		OrderTrackerMap StopBids_;
		OrderTrackerMap StopAsks_;

	private:
		static std::mutex m;
	};

}