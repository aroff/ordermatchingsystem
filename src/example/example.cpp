// example.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h"
#include <iostream>
#include <thread>

#include "../oms/OrderBook.h"
#include "example.h"

int main()
{
    std::cout << "Order Matching System!\n"; 

	OMS::Asset asset("NRGBTC");
	OMS::OrderBook orderBook(asset);


	// Ask Market order
	orderBook.add(OMS::LimitOrder::Create(OMS::Order::Side::Buy, 30, 100));
	orderBook.add(OMS::LimitOrder::Create(OMS::Order::Side::Buy, 20, 100));
	orderBook.add(OMS::LimitOrder::Create(OMS::Order::Side::Buy, 10, 100));

	// Bid market oder
	orderBook.add(OMS::LimitOrder::Create(OMS::Order::Side::Sell, 20, 300));
	orderBook.print();
	orderBook.add(OMS::LimitOrder::Create(OMS::Order::Side::Buy, 20, 100));
	orderBook.print();

	// invalid market order: zero quantity
	orderBook.add(OMS::MarketOrder::Create(OMS::Order::Side::Sell, 0));

	// Invalid limit Order: zero price
	orderBook.add(OMS::LimitOrder::Create(OMS::Order::Side::Sell, 0, 500));
}

