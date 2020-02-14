#pragma once

#include <cstdlib>
#include <cstdint>
#include <stdexcept>

namespace OMS {

	// Some type definitions. Note that I'm using uint32_t to avoid leading with floating arithmetic
	typedef uint32_t Price;
	typedef uint32_t Quantity;
	typedef uint32_t Cost;
	typedef int32_t DeltaQuantity;
	
}