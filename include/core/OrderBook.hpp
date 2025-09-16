#pragma once
#include <map>
#include <unordered_map>
#include "Order.hpp"
#include "Trade.hpp"
#include "OrderModify.hpp"

class OrderBook
{
    struct OrderEntry
    {
        OrderPointer order { nullptr };
        OrderPointers::iterator location {};
    };

    std::map<Price, OrderPointers, std::greater<> > bids; // Best bid
    std::map<Price, OrderPointers, std::less<> > asks; // Best ask
    std::unordered_map<OrderId, OrderEntry> orders;

    const Price& GetBestBid() const;
    const Price& GetBestAsk() const;
    bool CanMatch(const Side& side,const Price& price) const;
    Trades MatchOrders();

public:
    Trades AddOrder(const Order& order);
    Trades AddOrder(const OrderPointer& order);
    bool CancelOrder(const OrderId& orderId);
    Trades ModifyOrder(const OrderModify& orderModify);
};
