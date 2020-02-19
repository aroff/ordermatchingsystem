#include "OrderBook.h"
#include <algorithm>
#include <iostream>
#include <typeinfo>
#include <numeric>
#include <cmath>

std::mutex OMS::OrderBook::m;

OMS::OrderBook::OrderBook(const Asset & asset, const Asset &baseAsset) :
		asset_(asset)
	,	baseAsset_(baseAsset)
{
}

OMS::OrderBook::~OrderBook()
{
}

const OMS::Asset &OMS::OrderBook::asset() const
{
	return asset_;
}

void OMS::OrderBook::add_limit_order(OrderPtr order) {

	OMS::OrderTracker tracker(order);

	auto &queue = order->side() == OMS::Side::Buy ? Bids_ : Asks_;
	//ComparablePrice price(order.side(), order.limit());
	//queue.insert({ price, tracker });
	try_match_limit(tracker);
}

void OMS::OrderBook::add_market_order(OrderPtr order)
{
	OMS::OrderTracker tracker(order);

	auto queue = order->side() == OMS::Side::Buy ? Bids_ : Asks_;
	try_match_market(tracker);
}

void OMS::OrderBook::add_stoplimit_order(OrderPtr order)
{
	OMS::OrderTracker tracker(order);
	auto queue = order->side() == OMS::Side::Buy ? StopBids_ : StopAsks_;
	//ComparablePrice price(order.side(), order.limit());
	//queue.insert({ price, tracker });
	try_match_limit(tracker);
}

// Add new order to order book
bool OMS::OrderBook::add(OrderPtr order)
{
	std::unique_lock<std::mutex> guard(OrderBook::m); // lock order books from other threads

	auto err = order->validate();
	if(err != OMS::OrderError::Ok)
	{
		OnReject(*order, err);
		return false;
	}

	if (typeid(*order) == typeid(MarketOrder))
		add_market_order(order);
	else if (typeid(*order) == typeid(LimitOrder))
		add_limit_order(order);
	else if (typeid(*order) == typeid(StopLimitOrder))
		add_stoplimit_order(order);
	else
		throw std::runtime_error("Unknown order type");

	guard.unlock(); // unlock

	OnAccept(*order);

	return true;
}

void OMS::OrderBook::try_match_market(OrderTracker &tracker) {

	const LimitOrder &limitOrder = (const LimitOrder &)tracker.order();

	auto &asks = limitOrder.side() == Side::Buy ? Asks_ : Bids_;
	auto &bids = limitOrder.side() == Side::Buy ? Bids_ : Asks_;

	DeferredMatches deferred_aons;
	bool matched = match_order(tracker, asks, deferred_aons);
}

void OMS::OrderBook::try_match_limit(OrderTracker &tracker) {

	//auto a = (*tracker.order());;
	auto limitOrder = std::static_pointer_cast<LimitOrder>(tracker.order());

	//auto limitOrder = (std::shared_ptr<LimitOrder> &)a;

	auto &asks = limitOrder->side() == OMS::Side::Buy ? Asks_ : Bids_;
	auto &bids = limitOrder->side() == OMS::Side::Buy ? Bids_ : Asks_;

	DeferredMatches deferred_aons;
	bool matched = match_order(tracker, asks, deferred_aons);
	if (!tracker.is_filled() && !(limitOrder->filling() == OMS::Filling::ImmediateOrCancel)) // if we couldn't fill entire order immediately, add it to order queue. IOC orders are not added to order queue
	{
		ComparablePrice price(limitOrder->side(), limitOrder->limit());
		bids.insert({ price, tracker });
	}

}

// Cancel an order
bool OMS::OrderBook::cancel(OrderPtr order)
{
	return false;
}

// replace parameters in order book
void OMS::OrderBook::replace(OrderPtr order, Quantity deltaQuantity, Price newPrice)
{
	// TODO: check if better simply cancel/add new order
}

void OMS::OrderBook::set_price(Price price)
{
	price_ = price;
	
	//TODO: implement checking of stop orders, given that we have a new price

	//print();
}

OMS::Price OMS::OrderBook::get_price() const
{
	return price_;
}

OMS::Quantity OMS::OrderBook::do_trade(OrderTracker & income, OrderTracker & order,	Quantity maxQuantity)
{
	Price cross_price(0);	

	if (typeid(*income.order()) == typeid(LimitOrder)) {
		auto order = std::static_pointer_cast<LimitOrder> (income.order());
		cross_price = order->limit();
	}
	else if(typeid(*income.order()) == typeid(MarketOrder))
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
	if (tracker.order()->filling() == OMS::Filling::AllOrNothing)
		return match_aon_order(tracker, current_orders, deferred_aons); // all or nothing order

	return match_standard_order(tracker, current_orders, deferred_aons); // standard order
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

bool OMS::OrderBook::match_standard_order(OMS::OrderTracker &income, OrderTrackerMap& orders, DeferredMatches & deferred_aons)
{

	bool matched = false;
	Quantity income_quantity = income.open_quantity();
	OMS::Price orderPrice(0);

	if(typeid(*income.order()) == typeid(LimitOrder)) {
		auto order = std::static_pointer_cast<LimitOrder> (income.order());
		orderPrice = order->limit();
	}
	
	while (!orders.empty()) {
		auto entry = orders.begin();
		OrderTracker &order = entry->second;
		
		//TODO: handle the case which buyer and seller are same agents
		//if (order.order()->trader() == income.order()->trader())
		//	todo todo

		auto price = entry->first;
		if(!price.match(orderPrice))
			break;

		// We have at least a matching case
		Quantity current_quantity = order.open_quantity();

		Quantity traded = do_trade(income, order, UINT32_MAX);
		if (traded > 0) {
			matched = true;
			if (order.is_filled())
				orders.erase(entry); // remove order from book
			income_quantity -= traded; // update open quantity
		}
		else
			break;
	}

	return matched;
}

void printDepth(std::string name, const OMS::Asset &asset, const OMS::OrderTrackerMap &orderTrackerMap) {

	if (orderTrackerMap.empty())
		return;

	auto minPrice = (*orderTrackerMap.rbegin()).first.price();
	auto maxPrice = (*orderTrackerMap.begin()).first.price();

	std::cout << "****************** " << name << "****************** " << std::endl;
	//std::cout << "MinPrice: " << minPrice << ". MaxPrice: " << maxPrice << std::endl;

	/// count elements in price interval 
	std::vector<OMS::Quantity> depth(10);
	int i = 0;
	OMS::Price binStep = (maxPrice - minPrice) / 10;
	if (binStep == 0)
		binStep = 1;
	OMS::Price startPrice = minPrice, endPrice = maxPrice;
	OMS::Quantity totalQuantity(1);
	for (OMS::Price binPrice = startPrice; i < 10; binPrice += binStep) {
		int cnt = std::accumulate(orderTrackerMap.begin(), orderTrackerMap.end(), 0, [binPrice](int v, const OMS::OrderTrackerMap::value_type& p) {
			auto price = p.first.price();
			if (price >= binPrice && price < (binPrice + 1))
				return v + p.second.order()->quantity();
			else
				return v + OMS::Quantity(0);
		});
		depth[i++] = cnt;
		totalQuantity += cnt;
	}

	for (int i = 0; i < 10; ++i) {
		std::cout << double(startPrice + i) / pow(10.0, asset.decimalPlaces()) << "-" << double(startPrice + i + 1) / pow(10.0, asset.decimalPlaces()) << ": ";
		std::cout << std::string((depth[i] * 50) / totalQuantity, '*') << std::endl;
	}

}

void OMS::OrderBook::print() const {

	std::cout << " *********" << "ORDER BOOK FOR: " << asset_.ticker() << ". Price: " << double(price_) / pow(10.0, asset_.decimalPlaces()) << std::endl;
	double buyPrice = 0, sellPrice = 0;
	if(Bids_.begin() != Bids_.end())
		buyPrice = double(std::static_pointer_cast<LimitOrder>(Bids_.begin()->second.order())->limit()) / pow(10.0, asset_.decimalPlaces());
	if (Asks_.begin() != Asks_.end())
		sellPrice = double(std::static_pointer_cast<LimitOrder>(Asks_.begin()->second.order())->limit()) / pow(10.0, asset_.decimalPlaces());
	
	std::cout << "Buys: " << buyPrice << std::endl;
	std::cout << "Sells: " << sellPrice << std::endl;

	/*std::cout << "BIDS: " << std::endl;
	for (auto tracker : Bids_)
		std::cout << (tracker.second.order()->side() == Order::Side::Buy ? "BUY" : std::string("SELL")) << " " << tracker.second.open_quantity() << "/" << tracker.second.order()->quantity() << std::endl;
	
	std::cout << "ASKS: " << std::endl;
	for (auto tracker : Asks_)
		std::cout << (tracker.second.order()->side() == Order::Side::Buy ? "BUY" : std::string("SELL")) << " " << tracker.second.open_quantity() << "/" << tracker.second.order()->quantity() << std::endl;
		*/

	auto openSellOrders = std::accumulate(Asks_.begin(), Asks_.end(), 0, [](int v, const OrderTrackerMap::value_type& p) { return v + p.second.order()->quantity(); });
	auto openBuyOrders = std::accumulate(Bids_.begin(), Bids_.end(), 0, [](int v, const OrderTrackerMap::value_type& p) { return v + p.second.order()->quantity(); });
	std::cout << "Buy\t" << Bids_.size() << "\tOpen: " << openBuyOrders << " orders" << std::endl;
	std::cout << "Sell\t" << Asks_.size() << "\tOpen: " << openSellOrders << " orders" << std::endl;

	printDepth("Buy", asset_, Bids_);
	printDepth("Sell", asset_, Asks_);

}
void OMS::OrderBook::OnAccept(const Order & order)
{
	//std::cout << (order.side() == Order::Side::Buy ? "BUY" : std::string("SELL")) << " " << order.quantity() << " was accept into the queue." << std::endl;
}

void OMS::OrderBook::OnReject(const Order & order, OMS::OrderError err)
{
	std::cout << (order.side() == Side::Buy ? "BUY" : std::string("SELL")) << " " << order.quantity() << " was rejected." << std::endl;
}

void OMS::OrderBook::OnFill(const Order & order, const Order & matchedOrder, Quantity filledQuantity, Cost fillCost, bool incomeOrderFilled, bool matchedOrderFilled)
{
	
}

void OMS::OrderBook::OnCancel(const Order & order)
{
	std::cout << (order.side() == Side::Buy ? "BUY" : std::string("SELL")) << " " << order.quantity() << " was canceled." << std::endl;
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
