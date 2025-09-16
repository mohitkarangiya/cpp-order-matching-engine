#pragma once
#include <vector>
#include <utils/SPSCQueue.hpp>
#include "Order.hpp"

class Router
{
public:
    Router(const size_t& numSymbols,const size_t& queueCapacity);
    bool RouteOrder(const Order& order);
    SPSCQueue<Order>* GetQueueForSymbol ( const SymbolId& symbolId ) const;
private:
    std::vector<std::unique_ptr<SPSCQueue<Order>>> symbolQueues_;
    size_t numSymbols_ = 0;
};
