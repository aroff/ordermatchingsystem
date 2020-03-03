#include "OrderTracker.h"

OMS::OrderTracker::OrderTracker(OrderPtr order) :
		m_Order(order)
	,	openQuantity_(order->quantity())
	,	m_Reserved(0)
{
}

OMS::OrderTracker::~OrderTracker()
{
}

void OMS::OrderTracker::changeQuantity(DeltaQuantity delta)
{
	if (delta < 0 && (std::abs(delta) > openQuantity_))
		throw std::runtime_error("Quantity change larger than open quantity");

	openQuantity_ += delta;	
}

void OMS::OrderTracker::fill(Quantity quantity)
{
	if(quantity > openQuantity_)
		throw std::runtime_error("Fill size larger than open quantity");

	openQuantity_ -= quantity;
}

bool OMS::OrderTracker::is_filled() const
{
	return openQuantity_ == 0;
}

OMS::Quantity OMS::OrderTracker::filled_quantity() const
{
	return m_Order->quantity() - openQuantity_;
}

OMS::Quantity OMS::OrderTracker::open_quantity() const
{
	return openQuantity_ - m_Reserved;
}

OMS::Quantity OMS::OrderTracker::reserve(int32_t reserved) 
{
	m_Reserved += reserved;
	return openQuantity_ - m_Reserved;
}

OMS::OrderPtr OMS::OrderTracker::order() const
{
	return m_Order;
}

/*OMS::Order& OMS::OrderTracker::order()
{
	return m_Order;
}*/

//OMS::Order & OMS::OrderTracker::order()
//{
//	// TODO: inserir instrução de retorno aqui
//}
