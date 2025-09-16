#include "core/SymbolWorker.hpp"

#include <queue>

SymbolWorker::SymbolWorker(const SymbolId& symbolId, SPSCQueue<Order>* queue):
    symbolId_(symbolId),
    orderQueue_(queue)
{}

SymbolWorker::~SymbolWorker()
{
}

void SymbolWorker::Start()
{
    running_.store(true, std::memory_order_release);
    workerThread_ = std::jthread ( &SymbolWorker::ProcessOrders,this );
}

void SymbolWorker::Stop()
{
    running_.store(false);
    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

void SymbolWorker::ProcessOrders()
{
    while (running_.load())
    {
        const std::optional<Order> order = orderQueue_->pop();
        if ( order )
        {
            Trades trades = orderBook_.AddOrder(*order);
            if (!trades.empty())
            {
                //todo
                //try sending to notification queue
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
