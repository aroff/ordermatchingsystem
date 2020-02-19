# An Order Matching System in C++

This is a playground project for helping me understanding the workings of a real-time order book. 

# Main components

- Trader, represents the owner of the trading order. Each trader has an array with the balance in each currency (required for checking if it has funds for buying assets or using as margin)
- Asset / Future / Option, represents the asset being traded
- Order / LimitOrder / StopLimitOrder / OneCancelOtherOrder, represents a trading order
- OrderTracker, tracks execution (filling) of each order.
- OrderBook, maintains the entire order books with bids, asks, stop bids, stop asks, calls the matching algorithm and dispatch order related events and latest market price
- Depth, a real-time aggregator that consolidates the order book data and dispatch real-time events (usually consumed by client apps)

# Simple example

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

# Asset Types

- Simple assets, like stocks
- Futures
- Plain vanilla options, (european/american)

# Order types

- Market Order
- Limit order
- Stop Limit order (not implemented yet)
- One Cancel Other order (not implemented yet)

