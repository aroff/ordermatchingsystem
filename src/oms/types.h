#pragma once

#include <cstdlib>
#include <cstdint>
#include <stdexcept>

namespace OMS {

	// Some type definitions. Note that I'm using uint32_t to avoid leading with floating arithmetic
	typedef uint32_t Price;
	
	
	/* if we assume that it will not exists non-trading days, 
	https://en.wikipedia.org/wiki/Julian_day
	*/

	typedef uint32_t Quantity;
	typedef uint32_t Cost;
	typedef int32_t DeltaQuantity;

	class DateTime {
	public:
		
		DateTime(unsigned int year, unsigned int mo, unsigned int day, unsigned int hour, unsigned int minutes, unsigned int seconds) {
			_internalDate = 0; // compute date, assumine GMT and no holidays
		}


		static DateTime now() {
			return DateTime(2020, 2, 20, 0, 0, 0);//TODO MOCKUP
		}		

	protected:
		double _internalDate;
	};
}