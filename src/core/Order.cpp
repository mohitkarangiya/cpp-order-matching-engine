#include "core/Order.hpp"

bool Order::IsFilled() const
{
    return GetRemainingQuantity() == 0;
}

void Order::Fill(const Quantity& fillQuantity)
{
    if (fillQuantity > GetRemainingQuantity())
    {
        throw std::logic_error(std::format("Order ({}) cannot be filled. Fill amount greater than remaining quantity",GetOrderId()));
    }
    remainingQuantity -= fillQuantity;
}