#include "Asset.h"

OMS::Asset::Asset(const std::string &ticker, unsigned int decimalPlaces) :
		ticker_(ticker)
	, decimalPlaces_(decimalPlaces)
{
}


OMS::Asset::~Asset()
{
}

OMS::Option::Option(const std::string &ticker, const Asset &underlying, Price strike, DateTime expirationDate, OptionType type) :
		OMS::Asset(ticker)
	,	underlying_(underlying)
	,	strike_(strike)
	,	expirationDate_(expirationDate)
	,	type_(type)
{

}

OMS::Future::Future(const std::string &ticker, const Asset & underlying, DateTime expirationDate) :
		Asset(ticker)
	,	underlying_(underlying)
	,	expirationDate_(expirationDate)
{
}
