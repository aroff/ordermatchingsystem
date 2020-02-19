#pragma once
#include <string>
#include "types.h"

namespace OMS {
	class Asset
	{
	public:
		Asset(std::string ticker, unsigned int decimalPlaces = 8);
		virtual ~Asset();
		std::string ticker() const { return ticker_; }
		unsigned int decimalPlaces() const {			return decimalPlaces_;		}

	protected:
		std::string ticker_;
		unsigned int decimalPlaces_;
	};

	class Future : Asset {
	public:
		Future(std::string ticker, const Asset &underlying, Date expirationDate);
		const Asset &underlying() const { return underlying_; }

	protected:
		const Asset &underlying_;
		Date expirationDate_;
	};

	class Option : Asset {
	public:
		enum class OptionType {
			Call,
			Put
		};
		Option(std::string ticker, const Asset &underlying, Price strike, Date expirationDate, OptionType type);
		const Asset &underlying() const { return underlying_; }
		Price strike() const { return strike_; }
		Date expiration() const { return expirationDate_; }
		OptionType type() const { return type_; }

	protected:
		const Asset &underlying_;
		Price strike_;
		Date expirationDate_;
		OptionType type_;
	};
}