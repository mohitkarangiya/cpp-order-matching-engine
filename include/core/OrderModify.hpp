#pragma once
#include "Order.hpp"
#include <memory>

class OrderModify
{
public:
    OrderModify( const OrderId& orderId, const Side& side, const Price& price, const Quantity& quantity):
        orderId{orderId},
        side(side),
        price(price),
        quantity(quantity)
    { }

    const OrderId& GetOrderId() const { return orderId; }
    const Side& GetSide() const { return side; }
    const Price& GetPrice() const { return price; }
    const Quantity& GetQuantity() const { return quantity; }

    OrderPointer ToOrderPointer(OrderType type) const
    {
        return std::make_shared<Order>(orderId, type, side, price, quantity);
    }
private:
    OrderId orderId;
    Side side;
    Price price;
    Quantity quantity;
};