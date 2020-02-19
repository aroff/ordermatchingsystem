#include "stdafx.h"
#include "CppUnitTest.h"

#include <iostream>
#include <thread>

#include "../../src/oms/OrderBook.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace omstest
{		
	TEST_CLASS(OMSTests)
	{
	public:
		
		TEST_METHOD(InvalidQuantity)
		{
			OMS::Asset nrg("NRG");
			OMS::Asset btc("BTC");
			OMS::OrderBook orderBook(nrg, btc);
			OMS::Trader trader(1);

			// invalid market order: zero quantity
			Assert::AreEqual(orderBook.add(OMS::MarketOrder::Create(trader, OMS::Side::Sell, 0)), false);
		}

		TEST_METHOD(InvalidPrice)
		{
			OMS::Asset nrg("NRG");
			OMS::Asset btc("BTC");
			OMS::OrderBook orderBook(nrg, btc);
			OMS::Trader trader(1);

			// Invalid limit Order: zero price
			Assert::AreEqual(orderBook.add(OMS::LimitOrder::Create(trader, OMS::Side::Sell, 0, 500)), false);
		}

	};
}