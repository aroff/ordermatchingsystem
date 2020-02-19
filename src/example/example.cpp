// example.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>

#include "../oms/OrderBook.h"
#include "example.h"

std::random_device random_device;
std::mt19937 random_engine(random_device());
std::normal_distribution<double> zScore(0, 1);
double price(double oldPrice, double volatility, double t) {
	auto z = zScore(random_engine);
	auto p = oldPrice * exp(t * z);
	return p;
}

void SimulateMarket(OMS::OrderBook *orderBook) {

	OMS::Trader trader1(1);
	OMS::Trader trader2(2);

	std::vector<std::thread> t_orders;
	
	//std::uniform_int_distribution<int> priceDistribution(30, 35);
	std::uniform_int_distribution<int> quantityDistribution(100, 200);

	OMS::Price buyPrice = 2000100;
	OMS::Price sellPrice = 200000;

	while (true)
	{
		buyPrice = OMS::Price(price(buyPrice, 1 /*Annual volatility*/, 2e-3 /* t */));
		sellPrice = OMS::Price(price(sellPrice, 1 /*Annual volatility*/, 2e-3 /* t */));

		//	std::cout << "Buy: " << buyPrice << "\tSell: " << sellPrice << std::endl;

		OMS::Quantity buyQuantity = quantityDistribution(random_engine);
		OMS::Quantity sellQuantity = quantityDistribution(random_engine);

		t_orders.push_back(std::thread([&] {
			orderBook->add(OMS::LimitOrder::Create(trader1, OMS::Side::Buy, buyPrice, buyQuantity));

			orderBook->add(OMS::LimitOrder::Create(trader2, OMS::Side::Sell, sellPrice, sellQuantity));
		}));

		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		orderBook->print();

	}

}

int main()
{
    std::cout << "Order Matching System!\n"; 

	OMS::Asset nrg("NRG", 5);	// Energi (NRG)
	OMS::Asset btc("BTC", 5);	// Bitcoin (BTC)
	OMS::Asset usdt("USDT", 5);	// USD Thether (USDT)
	OMS::Option optionBTC("OPTBTC100000", btc, 10000, OMS::Date(2021, 01, 01, 0, 0, 0), OMS::Option::OptionType::Call); // Call option with strike at 10,000 USD

	OMS::OrderBook orderBook(nrg, btc);	

	SimulateMarket(&orderBook);
	/*OMS::Trader trader1(1);
	OMS::Trader trader2(2);
	OMS::Trader trader3(3);

	while (true) {
		std::string op, tr;
		OMS::Price limit;
		OMS::Quantity qtt;
		OMS::Side side;
		std::cin >> tr >> op >> limit >> qtt;
		if (op == "buy")
			side = OMS::Side::Buy;
		else
			side = OMS::Side::Sell;
		OMS::Trader *trader=0;
		if (tr == "t1")
			trader = &trader1; 
		else if (tr == "t2")
			trader = &trader2; 
		else if (tr == "t3")
			trader = &trader3;
		else
		{
			std::cout << "invalid trader" << std::endl;
			continue;
		}
		orderBook.add(OMS::LimitOrder::Create(*trader, side, limit, qtt));
		orderBook.print();
	}
	*/
	

	// Sell limit oder
	//orderBook.add(OMS::LimitOrder::Create(trader2, OMS::Side::Sell, 50, 20));
	//orderBook.print();

	//orderBook.add(OMS::LimitOrder::Create(trader2, OMS::Side::Buy, 70, 10));
	//orderBook.print();
}

