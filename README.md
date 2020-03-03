# An Order Matching System in C++

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/75db636aa855419b8075d9e9d7c26551)](https://app.codacy.com/manual/aroff/ordermatchingsystem?utm_source=github.com&utm_medium=referral&utm_content=aroff/ordermatchingsystem&utm_campaign=Badge_Grade_Dashboard)

## Main components

-  Trader, represents the owner of the trading order. Each trader has an array with the balance in each currency (required for checking if it has funds for buying assets or using as margin)
-  Asset / Future / Option, represents the asset being traded
-  Order / LimitOrder / StopLimitOrder / OneCancelOtherOrder, represents a trading order
-  OrderTracker, tracks execution (filling) of each order.
-  OrderBook, maintains the entire order books with bids, asks, stop bids, stop asks, calls the matching algorithm and dispatch order related events and latest market price
-  Depth, a real-time aggregator that consolidates the order book data and dispatch real-time events (usually consumed by client apps)

## Simple example

```cpp
OMS::Trader trader1(1);
OMS::Trader trader2(2);

OMS::Asset nrg("NRG", 5);	// Energi (NRG)
OMS::Asset usd("USDT", 5);	// US Dollar (USD)

OMS::OrderBook orderBook(nrg, usd); // Create a order book for pair NRG x USD

// Add a new limit order: Trader 1 wants to buy 100 units of NRG by paying 2 USDT
orderBook.add(OMS::LimitOrder::Create(trader1, OMS::Order::Side::Buy, 2 /*bid price*/, 100 /* quantity */));
orderBook.print();

// Add a new limit order: Trader 2 wants to sell 110 units of NRG by paying 2 USDT
orderBook.add(OMS::LimitOrder::Create(trader2, OMS::Order::Side::Sell, 2.1 /*ask price*/, 110 /* quantity */));
orderBook.print();

```

## Asset Types

-  Simple assets, like stocks
-  Futures
-  Plain vanilla options, (european/american)

## Order types

-  Market Order
-  Limit order
-  Stop Limit order (not implemented yet)
-  One Cancel Other order (not implemented yet)

## Adding orders to the Order book

To prevent 
```cpp
std::mutex OMS::OrderBook::m;
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
```

## Order Matching Order

Orders are processed in the following order: Cancel orders are processed first, followed by market order, limit orders and stop order. 

This section discusses the rules used by the order matching engine to execute order. 

Orders may be partially filled or not filled at all (in the case of a limit order).

After cancel orders, market orders are given the highest priority and will execute immediately when an opposite order exists in the market. Market orders may be partially filled, at different prices.

An attempt is always made to match a buy order first, against existing sell orders.

## How it works

Limit orders are sorted:

Buy orders are sorted in descending order by their bid price and ascending order by time stamp for orders that have the same price. Orders with the highest bid (buy) price are kept at the top of the queue and will be executed first. For equal priced bids, the order that arrives first is executed first.

Sell orders are sorted in ascending order by their ask price, and like buy orders, by ascending order by time stamp for orders with the same price. Orders with the lowest sell (ask) prices will be sold first. For orders with the same ask price, the order that arrives first will be sold first.

When an order completes, either because it is filled, canceled or expires, a status message is returned to the agent that submitted the order. Whenever there is a market transaction there is a risk that a counter party will default on the transaction. All transactions take place within the order matching engine.
