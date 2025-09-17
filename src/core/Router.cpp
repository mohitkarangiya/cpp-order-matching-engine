#include "core/Router.hpp"

Router::Router(const size_t& numSymbols,const size_t& queueCapacity):
    numSymbols_(numSymbols)
{
    symbolQueues_.reserve(numSymbols);
    for ( size_t i = 0; i < numSymbols; ++i )
    {
        symbolQueues_.push_back(std::make_unique<SPSCQueue<OrderMessage>>(queueCapacity));
    }
}

bool Router::RouteMessage(const SymbolId& symbolId, const OrderMessage& message)
{
    SPSCQueue<OrderMessage>* queue = GetQueueForSymbol(symbolId);
    if(queue != nullptr) return queue->push(message);
    return false;
}

SPSCQueue<OrderMessage>* Router::GetQueueForSymbol(const SymbolId& symbolId) const
{
    if( symbolId < 1 || symbolId > numSymbols_ ) return nullptr;
    return symbolQueues_[symbolId-1].get();
}
