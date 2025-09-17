#include "core/SymbolWorker.hpp"

#include <iostream>
#include <queue>
#include "core/OrderMessage.hpp"

SymbolWorker::SymbolWorker(const SymbolId& symbolId, SPSCQueue<OrderMessage>* queue, MPSCQueue<std::string>* logQueue):
    symbolId_(symbolId),
    orderQueue_(queue),
    logQueue_(logQueue)
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
        const std::optional<OrderMessage> message = orderQueue_->pop();
        if ( message )
        {
            if (std::holds_alternative<AddOrderMessage>(*message))
            {
                const auto& [order] = std::get<AddOrderMessage>(*message);
                logQueue_->push("Worker " + std::to_string(symbolId_) + ": Processing AddOrder ID " + std::to_string(order.GetOrderId()));
                const Trades trades = orderBook_.AddOrder(std::make_shared<Order>(order));
                if (!trades.empty())
                {
                    logQueue_->push("Worker " + std::to_string(symbolId_) + ": Generated " + std::to_string(trades.size()) + " trades:");
                    for (const auto& trade : trades)
                    {
                        logQueue_->push( "  Trade: " + std::to_string(trade.GetBidTradeInfo().quantity)
                                  + " shares at price " + std::to_string(trade.GetBidTradeInfo().price)
                                  + " (Buy Order ID: " + std::to_string(trade.GetBidTradeInfo().orderId)
                                  + ", Sell Order ID: " + std::to_string(trade.GetAskTradeInfo().orderId) + ")");
                    }
                }
            }
            else if (std::holds_alternative<ModifyOrderMessage>(*message))
            {
                const auto& [orderModify] = std::get<ModifyOrderMessage>(*message);
                std::cout << "Worker " << symbolId_ << ": Processing ModifyOrder ID " << orderModify.GetOrderId() << std::endl;
                const Trades trades = orderBook_.ModifyOrder(orderModify);
                if (!trades.empty())
                {
                    logQueue_->push("Worker " + std::to_string(symbolId_) + ": Modify generated " + std::to_string(trades.size()) + " trades:");
                    for (const auto& trade : trades)
                    {
                        logQueue_->push( "  Trade: " + std::to_string(trade.GetBidTradeInfo().quantity)
                                  + " shares at price " + std::to_string(trade.GetBidTradeInfo().price)
                                  + " (Buy Order ID: " + std::to_string(trade.GetBidTradeInfo().orderId)
                                  + ", Sell Order ID: " + std::to_string(trade.GetAskTradeInfo().orderId) + ")");
                    }
                }
            }
            else if (std::holds_alternative<CancelOrderMessage>(*message))
            {
                const auto& [OrderId] = std::get<CancelOrderMessage>(*message);
                logQueue_->push("Worker " + std::to_string(symbolId_) + ": Processing CancelOrder ID " + std::to_string(OrderId));
                if (orderBook_.CancelOrder(OrderId))
                {
                    logQueue_->push("Worker " + std::to_string(symbolId_) + ": Order " + std::to_string(OrderId) + " cancelled successfully");
                }
                else
                {
                    logQueue_->push("Worker " + std::to_string(symbolId_) + ": Failed to cancel order " + std::to_string(OrderId));
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}