#pragma once
#include "Order.hpp"
#include "OrderModify.hpp"
#include <variant>

struct AddOrderMessage
{
    Order order;
};

struct ModifyOrderMessage
{
    OrderModify orderModify;
};

struct CancelOrderMessage
{
    OrderId orderId;
};

using OrderMessage = std::variant<AddOrderMessage,ModifyOrderMessage,CancelOrderMessage>;