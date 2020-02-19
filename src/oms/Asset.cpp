#include "Asset.h"

OMS::Asset::Asset(std::string ticker, unsigned int decimalPlaces) :
		ticker_(ticker)
	, decimalPlaces_(decimalPlaces)
{
}


OMS::Asset::~Asset()
{
}

OMS::Option::Option(std::string ticker, const Asset &underlying, Price strike, Date expirationDate, OptionType type) :
		OMS::Asset(ticker)
	,	underlying_(underlying)
	,	strike_(strike)
	,	expirationDate_(expirationDate)
	,	type_(type)
{

}

OMS::Future::Future(std::string ticker, const Asset & underlying, Date expirationDate) :
		Asset(ticker)
	,	underlying_(underlying)
	,	expirationDate_(expirationDate)
{
}
