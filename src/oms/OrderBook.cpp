#include "OrderBook.h"
#include <algorithm>
#include <iostream>

OMS::OrderBook::OrderBook(const Asset & asset) :
		Asset_(asset)
{
}

OMS::OrderBook::~OrderBook()
{
}

const OMS::Asset &OMS::OrderBook::asset() const
{
	return Asset_;
}

void OMS::OrderBook::add_limit_order(const LimitOrder &order) {

	OMS::OrderTracker tracker(order);

	auto &queue = order.side() == OMS::Order::Buy ? Bids_ : Asks_;
	//ComparablePrice price(order.side(), order.limit());
	//queue.insert({ price, tracker });
	try_match_limit(tracker);
}

void OMS::OrderBook::add_market_order(const MarketOrder & order)
{
	OMS::OrderTracker tracker(order);

	auto queue = order.side() == OMS::Order::Buy ? Bids_ : Asks_;
	//ComparablePrice price(order.side(), 0);
	//queue.insert({ price, tracker });
	try_match_market(tracker);
}

void OMS::OrderBook::add_stoplimit_order(const StopLimitOrder & order)
{
	OMS::OrderTracker tracker(order);
	auto queue = order.side() == OMS::Order::Buy ? StopBids_ : StopAsks_;
	//ComparablePrice price(order.side(), order.limit());
	//queue.insert({ price, tracker });
	try_match_limit(tracker);
}

// Add new order to order book
void OMS::OrderBook::add(const Order & order)
{
	auto err = order.validate();
	if(err != OMS::OrderError::Ok)
	{
		OnReject(order, err);
		return;
	}

	if (order.type() == Order::Type::Market)
		add_market_order((const MarketOrder &)order);
	else if (order.type() == Order::Type::Limit)
		add_limit_order((const LimitOrder &)order);
	else if (order.type() == Order::Type::Limit)
		add_stoplimit_order((const StopLimitOrder &)order);
	else
		throw std::runtime_error("unknown order type");

	OnAccept(order);

	return;
}

void OMS::OrderBook::try_match_market(OrderTracker &tracker) {

	const LimitOrder &limitOrder = (const LimitOrder &)tracker.order();

	auto &asks = limitOrder.side() == Order::Side::Buy ? Asks_ : Bids_;
	auto &bids = limitOrder.side() == Order::Side::Buy ? Bids_ : Asks_;

	DeferredMatches deferred_aons;
	bool matched = match_order(tracker, asks, deferred_aons);
}

void OMS::OrderBook::try_match_limit(OrderTracker &tracker) {

	const LimitOrder &limitOrder = (const LimitOrder &) tracker.order();

	auto &asks = limitOrder.side() == Order::Side::Buy ? Asks_ : Bids_;
	auto &bids = limitOrder.side() == Order::Side::Buy ? Bids_ : Asks_;

	DeferredMatches deferred_aons;
	bool matched = match_order(tracker, asks, deferred_aons);
	if (!tracker.is_filled() && !(limitOrder.filling() == Order::ImmediateOrCancel)) // if we couldn't fill entire order immediately, add it to order queue. IOC orders are not added to order queue
	{
		ComparablePrice price(limitOrder.side(), limitOrder.limit());
		bids.insert({ price, tracker });
	}

}

// cancel existing order
void OMS::OrderBook::cancel(const Order & order)
{
}

// replace parameters in order book
void OMS::OrderBook::replace(const Order & order, Quantity deltaQuantity, Price newPrice)
{
	// TODO: check if better simply cancel/add new order
}

void OMS::OrderBook::set_price(Price price)
{
	OMS::Price oldPrice = price_;

	//bool buySide = price > oldPrice;  // price increase?
	//check_stop_orders(buySide, price/*, buySide ? m_StopBids | m_StopAsks*/);

	price_ = price;

	std::cout << "New market price: " << price_ << std::endl;
}

OMS::Price OMS::OrderBook::get_price() const
{
	return price_;
}

OMS::Quantity OMS::OrderBook::do_trade(OrderTracker & income, OrderTracker & order,	Quantity maxQuantity)
{
	Price cross_price(0);

	if(income.order().type() == Order::Type::Limit) {
		cross_price = ((const LimitOrder &)income.order()).limit();
	}
	else if(income.order().type() == Order::Type::Market)
		cross_price = price_;

	Quantity fill_qty = std::min(maxQuantity, std::min(income.open_quantity(), order.open_quantity()));
	if (fill_qty > 0) {
		income.fill(fill_qty);	// fill income order
		order.fill(fill_qty); // fill current order
		set_price(cross_price); // update order latest price
	}

	return fill_qty;
}

bool OMS::OrderBook::match_order(OMS::OrderTracker &tracker, OMS::OrderTrackerMap& current_orders, OMS::DeferredMatches & deferred_aons)
{
	if (tracker.order().filling() == OMS::Order::Filling::AllOrNothing)
		return match_aon_order(tracker, current_orders, deferred_aons); // all or nothing order

	return match_regular_order(tracker, current_orders, deferred_aons); // standard order
}

bool OMS::OrderBook::match_aon_order(OMS::OrderTracker &income, OrderTrackerMap& current_orders, DeferredMatches & deferred_aons)
{
// while incoming ! satisfied
//   current is reg->trade
//   current is AON:
//    incoming satisfies AON ->TRADE
//    add AON to deferred
// loop
	return false;
}

bool OMS::OrderBook::match_regular_order(OMS::OrderTracker &income, OrderTrackerMap& orders, DeferredMatches & deferred_aons)
{

	bool matched = false;
	Quantity income_quantity = income.open_quantity();
	OMS::Price orderPrice(0);

	if (income.order().type() == Order::Type::Limit) {
		orderPrice = ((const LimitOrder &)income.order()).limit();
	}
	
	while (!orders.empty()) {
		auto entry = orders.begin();
		auto price = entry->first;
		if(!price.match(orderPrice))
			break;

		// We have at least a matching case
		OrderTracker &order = entry->second;
		Quantity current_quantity = order.open_quantity();

		Quantity traded = do_trade(income, order, UINT32_MAX);
		if (traded > 0) {
			matched = true;
			if(order.is_filled())
				orders.erase(entry); // remove order from book
			income_quantity -= traded; // update open quantity
		}


	}

	return matched;
}

void OMS::OrderBook::print() const {

	std::cout << " *********" << "ORDER BOOK FOR: " << Asset_.ticker() << " *********" << std::endl;

	std::cout << "BIDS: " << std::endl;
	for (auto tracker : Bids_)
		std::cout << (tracker.second.order().side() == Order::Side::Buy ? "BUY" : std::string("SELL")) << " " << tracker.second.open_quantity() << "/" << tracker.second.order().quantity() << std::endl;
	
	std::cout << "ASKS: " << std::endl;
	for (auto tracker : Asks_)
		std::cout << (tracker.second.order().side() == Order::Side::Buy ? "BUY" : std::string("SELL")) << " " << tracker.second.open_quantity() << "/" << tracker.second.order().quantity() << std::endl;

}
void OMS::OrderBook::OnAccept(const Order & order)
{
	std::cout << (order.side() == Order::Side::Buy ? "BUY" : std::string("SELL")) << " " << order.quantity() << " was accept into the queue." << std::endl;
}

void OMS::OrderBook::OnReject(const Order & order, OMS::OrderError err)
{
	std::cout << (order.side() == Order::Side::Buy ? "BUY" : std::string("SELL")) << " " << order.quantity() << " was rejected." << std::endl;
}

void OMS::OrderBook::OnFill(const Order & order, const Order & matchedOrder, Quantity filledQuantity, Cost fillCost, bool inboundOrderFilled, bool matchedOrderFilled)
{
	
}

void OMS::OrderBook::OnCancel(const Order & order)
{
	std::cout << (order.side() == Order::Side::Buy ? "BUY" : std::string("SELL")) << " " << order.quantity() << " was canceled." << std::endl;
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
