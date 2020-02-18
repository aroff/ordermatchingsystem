#pragma once
#include <string>

namespace OMS {
	class Asset
	{
	public:
		Asset(std::string ticker);
		virtual ~Asset();
		std::string ticker() const { return ticker_; }

	protected:
		std::string ticker_;
	};
}