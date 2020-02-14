#pragma once
#include "Asset.h"
#include "Order.h"
#include "OrderTracker.h"
#include "ComparablePrice.h"

#include <map>
#include <list>

namespace OMS {

	typedef std::multimap<ComparablePrice, OrderTracker> OrderTrackerMap;
	typedef std::list<typename OrderTrackerMap::iterator> DeferredMatches;


	// This class represents the limit (maker) order book for a given asset
	class OrderBook
	{
	public:


		// Construct the Order book for Asset
		OrderBook(const Asset &asset);
		virtual ~OrderBook();

		const Asset asset() const;

		// Order management methods
		virtual void add(const Order &order/*, const OrderConditions orderConditions = 0*/);

		virtual void cancel(const Order &order);

		virtual void replace(const Order &order, Quantity deltaQuantity, Price newPrice);

		// set current market price for the Asset
		virtual void set_price(Price price);

		virtual Price get_price() const;

	protected:
		/// Events interfaces
		virtual void OnAccept(const Order &order, Quantity quantity);
		virtual void OnReject(const Order &order, std::string reason);
		virtual void OnFill(const Order &order, const Order &matchedOrder, Quantity filledQuantity, Cost fillCost, bool inboundOrderFilled, bool matchedOrderFilled);
		virtual void OnCancel(const Order &order, Quantity quantity);
		virtual void OnCancelReject(const Order &order, std::string reason);
		virtual void OnReplace(const Order &order, Quantity currentQuantity, Quantity newQuantity, Price newPrice);
		virtual void OnReplaceReject(const Order &order, std::string reason);
		virtual void OnTrade(const OrderBook &order, Quantity quantity, Cost cost);
		///

		virtual void OnOrderBookChange();

		// 

		//void check_stop_orders(bool side, Price price, )

	protected:

		// Matching
		virtual bool match_order(const OrderTracker &inbound, Price inboundPrice, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons);

		// matching of a regular order
		bool match_regular_order(const OrderTracker &inbound, Price inboundPrice, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons);


		// matching of an all or nothing order
		bool match_aon_order(const OrderTracker &inbound, Price inboundPrice, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons);


		const Asset &m_Asset;
		Price m_Price{ 0 };  // Market Price

		OrderTrackerMap m_Bids;
		OrderTrackerMap m_Asks;
		
		OrderTrackerMap m_StopBids;
		OrderTrackerMap m_StopAsks;

	};
}