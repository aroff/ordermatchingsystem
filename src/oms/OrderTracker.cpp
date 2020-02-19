#include "OrderTracker.h"

OMS::OrderTracker::OrderTracker(OrderPtr order) :
		m_Order(order)
	,	m_OpenQuantity(order->quantity())
	,	m_Reserved(0)
{
}

OMS::OrderTracker::~OrderTracker()
{
}

void OMS::OrderTracker::changeQuantity(DeltaQuantity delta)
{
	if (delta < 0 && (std::abs(delta) > m_OpenQuantity))
		throw std::runtime_error("Quantity change larger than open quantity");

	m_OpenQuantity += delta;	
}

void OMS::OrderTracker::fill(Quantity quantity)
{
	if(quantity > m_OpenQuantity)
		throw std::runtime_error("Fill size larger than open quantity");

	m_OpenQuantity -= quantity;
}

bool OMS::OrderTracker::is_filled() const
{
	return m_OpenQuantity == 0;
}

OMS::Quantity OMS::OrderTracker::filled_quantity() const
{
	return m_Order->quantity() - m_OpenQuantity;
}

OMS::Quantity OMS::OrderTracker::open_quantity() const
{
	return m_OpenQuantity - m_Reserved;
}

OMS::Quantity OMS::OrderTracker::reserve(int32_t reserved) 
{
	m_Reserved += reserved;
	return m_OpenQuantity - m_Reserved;
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
