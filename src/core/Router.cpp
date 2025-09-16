#include "core/Router.hpp"

Router::Router(const size_t& numSymbols,const size_t& queueCapacity):
    numSymbols_(numSymbols)
{
    symbolQueues_.reserve(numSymbols);
    for ( size_t i = 0; i < numSymbols; ++i )
    {
        symbolQueues_.push_back(std::make_unique<SPSCQueue<Order>>(queueCapacity));
    }
}

bool Router::RouteOrder(const Order& order)
{
    SPSCQueue<Order>* queue = GetQueueForSymbol(order.GetSymbolId());
    if(queue != nullptr) return queue->push(order);
    return false;
}

SPSCQueue<Order>* Router::GetQueueForSymbol(const SymbolId& symbolId) const
{
    if( symbolId < 1 || symbolId > numSymbols_ ) return nullptr;
    return symbolQueues_[symbolId-1].get();
}
