#pragma once
#include "Order.h"

namespace OMS {

	/// The order tracker object keep the state of the Order object inside an Order book. 
	/// 
	
	class OrderTracker
	{
	public:
		OrderTracker(OrderPtr order);
		virtual ~OrderTracker();

		/// Modify an order quantity
		void changeQuantity(DeltaQuantity delta);

		/// Fill an order
		void fill(Quantity quantity);

		/// Is filled?
		bool is_filled() const;

		/// total filled quantity
		Quantity filled_quantity() const;

		/// open quantity
		Quantity open_quantity() const;

		Quantity reserve(int32_t reserved);

		// Order accessors
		OrderPtr order() const;

	protected:
		OrderPtr m_Order;
		Quantity m_OpenQuantity;
		Quantity m_Reserved;
	};
}