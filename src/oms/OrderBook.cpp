#include "OrderBook.h"

OMS::OrderBook::OrderBook(const Asset & asset) :
		m_Asset(asset)
{
}

OMS::OrderBook::~OrderBook()
{
}

const Asset OMS::OrderBook::asset() const
{
	return Asset();
}

void OMS::OrderBook::add(const Order & order)
{
}

void OMS::OrderBook::cancel(const Order & order)
{
}

void OMS::OrderBook::replace(const Order & order, Quantity deltaQuantity, Price newPrice)
{
}

void OMS::OrderBook::set_price(Price price)
{
	OMS::Price oldPrice = m_Price;

	bool buySide = price > oldPrice;  // price increase?
	//check_stop_orders(buySide, price/*, buySide ? m_StopBids | m_StopAsks*/);

	m_Price = price;
}

OMS::Price OMS::OrderBook::get_price() const
{
	return m_Price;
}

bool OMS::OrderBook::match_order(const OMS::OrderTracker &inbound, OMS::Price inboundPrice, OMS::OrderTrackerMap& current_orders, OMS::DeferredMatches & deferred_aons)
{
	if (inbound.order().all_or_none())
		return match_aon_order(inbound, inboundPrice, current_orders, deferred_aons);

	return match_regular_order(inbound, inboundPrice, current_orders, deferred_aons);
}

bool OMS::OrderBook::match_aon_order(const OMS::OrderTracker &inbound, OMS::Price inboundPrice, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons)
{
	return false;
}

bool OMS::OrderBook::match_regular_order(const OMS::OrderTracker &inbound, OMS::Price inboundPrice, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons)
{
	// while incoming ! satisfied
	//   current is reg->trade
	//   current is AON:
	//    incoming satisfies AON ->TRADE
	//    add AON to deferred
	// loop
	bool matched = false;
	Quantity inbound_qty = inbound.open_quantity();
	
	typename OrderTrackerMap::iterator pos = current_orders.begin();


	return matched;
}

void OMS::OrderBook::OnAccept(const Order & order, Quantity quantity)
{
}

void OMS::OrderBook::OnReject(const Order & order, std::string reason)
{
}

void OMS::OrderBook::OnFill(const Order & order, const Order & matchedOrder, Quantity filledQuantity, Cost fillCost, bool inboundOrderFilled, bool matchedOrderFilled)
{
}

void OMS::OrderBook::OnCancel(const Order & order, Quantity quantity)
{
}

void OMS::OrderBook::OnCancelReject(const Order & order, std::string reason)
{
}

void OMS::OrderBook::OnReplace(const Order & order, Quantity currentQuantity, Quantity newQuantity, Price newPrice)
{
}

void OMS::OrderBook::OnReplaceReject(const Order & order, std::string reason)
{
}

void OMS::OrderBook::OnTrade(const OrderBook & order, Quantity quantity, Cost cost)
{
}

void OMS::OrderBook::OnOrderBookChange()
{
}
