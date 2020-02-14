#pragma once
#include "types.h"

namespace OMS {

	class Order
	{
	public:

		/// is this a limit order?
		bool is_limit() const;

		/// Is a buy order
		virtual bool is_buy() const = 0;

		/// Price of this order, if any
		virtual Price price() const = 0;

		/// Stop price of order, if any
		virtual Price stop() const = 0;

		virtual Quantity quantity() const = 0;

		// only confirm trade when completelly filled
		virtual bool all_or_none() const;

		// cancel any remaining quantity, if not executed on the market
		virtual bool imediate_or_cancel() const;

	protected:
		Order();
		virtual ~Order();
	};
}
