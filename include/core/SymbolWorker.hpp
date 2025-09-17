#pragma once
#include <thread>
#include "Order.hpp"
#include "OrderBook.hpp"
#include "OrderMessage.hpp"
#include "utils/SPSCQueue.hpp"

class SymbolWorker
{
public:
    SymbolWorker( const SymbolId& symbolId, SPSCQueue<OrderMessage>* queue );
    ~SymbolWorker();

    void Start();
    void Stop();

private:
    void ProcessOrders();

private:
    SymbolId symbolId_;
    OrderBook orderBook_;
    SPSCQueue<OrderMessage>* orderQueue_;
    std::jthread workerThread_;
    std::atomic<bool> running_{true};
};
