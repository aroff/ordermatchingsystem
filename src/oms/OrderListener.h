#pragma once
#include "Order.h"
#include <string>

namespace OMS {

	class OrderListener
	{
	public:
		OrderListener();
		virtual ~OrderListener();

		//Event for order accepted
		virtual void OnAccept(const Order &order) = 0;

		// Event for order rejected
		virtual void OnReject(const Order &order) = 0;

		// Event when order gets filled (matched)
		virtual void OnFill(const Order &order, const Order &matchedOrder, Quantity quantity, Cost cost) = 0;

		// Event for order canceled (initialed from client)
		virtual void OnCancel(const Order &order) = 0;

		// Event for cancel order rejection
		virtual void OnCancelReject(const Order &order, std::string reason) = 0;

		// Event for order replacing (changing order parameters)
		virtual void OnReplace(const Order &order, Quantity deltaQuantity, Price newPrice) = 0;

		// Event for order replace rejection
		virtual void OnReplaceReject(const Order &order, std::string reason) = 0;
	};


}