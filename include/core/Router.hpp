#pragma once
#include <vector>
#include <utils/SPSCQueue.hpp>
#include "Order.hpp"
#include "OrderMessage.hpp"

class Router
{
public:
    Router(const size_t& numSymbols,const size_t& queueCapacity);
    bool RouteMessage(const SymbolId& symbolId, const OrderMessage& message);
    SPSCQueue<OrderMessage>* GetQueueForSymbol ( const SymbolId& symbolId ) const;
private:
    std::vector<std::unique_ptr<SPSCQueue<OrderMessage>>> symbolQueues_;
    size_t numSymbols_ = 0;
};
