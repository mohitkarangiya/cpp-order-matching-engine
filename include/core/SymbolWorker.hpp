#pragma once
#include <thread>
#include "Order.hpp"
#include "OrderBook.hpp"
#include "utils/SPSCQueue.hpp"

class SymbolWorker
{
public:
    SymbolWorker( const SymbolId& symbolId, SPSCQueue<Order>* queue );
    ~SymbolWorker();

    void Start();
    void Stop();

private:
    void ProcessOrders();

private:
    SymbolId symbolId_;
    OrderBook orderBook_;
    SPSCQueue<Order>* orderQueue_;
    std::jthread workerThread_;
    std::atomic<bool> running_{true};
};
