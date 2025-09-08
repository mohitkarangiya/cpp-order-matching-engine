#pragma once
#include <cstdint>
#include <format>
#include <memory>
#include <list>

enum class Side: uint8_t { Buy = 0, Sell = 1 };
enum class OrderType: uint8_t { GoodTillCancel = 0, FillOrKill = 1 };

using Price = std::int64_t;
using Quantity = std::uint64_t;
using OrderId = std::uint64_t;

class Order
{
public:
    Order(const OrderId& orderId, const OrderType& orderType , const Side& side, const Price& price, const Quantity& quantity):
        orderId{orderId},
        orderType{orderType},
        side{side},
        price{price},
        initialQuantity{quantity},
        remainingQuantity{quantity}
    {}

    const OrderId& GetOrderId() const { return orderId; }
    const OrderType& GetOrderType() const { return orderType; }
    const Side& GetSide() const { return side; }
    const Price& GetPrice() const { return price; }
    const Quantity& GetInitialQuantity() const { return initialQuantity; }
    const Quantity& GetRemainingQuantity() const { return remainingQuantity; }
    Quantity GetFilledQuantity() const { return initialQuantity - remainingQuantity; }

    bool IsFilled() const;
    void Fill(const Quantity& fillQuantity);

private:
    OrderId orderId;
    OrderType orderType;
    Side side;
    Price price;
    Quantity initialQuantity;
    Quantity remainingQuantity;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;