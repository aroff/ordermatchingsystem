#pragma once
#include <cstdint>
#include <vector>
#include "types.h"

namespace OMS {
	class Trader
	{
	public:
		Trader();
		Trader(uint32_t traderId);
		virtual ~Trader();

		void OnBalanceChanged(const Trader &trader, uint32_t assetId, Quantity newBalance);

	protected:
		uint32_t traderId_;
		std::vector<uint32_t> balance_; // updated balance in each asset
	};
}