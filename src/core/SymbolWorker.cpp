#include "core/SymbolWorker.hpp"

#include <iostream>
#include <queue>
#include "core/OrderMessage.hpp"

SymbolWorker::SymbolWorker(const SymbolId& symbolId, SPSCQueue<OrderMessage>* queue):
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
        const std::optional<OrderMessage> message = orderQueue_->pop();
        if ( message )
        {
            if (std::holds_alternative<AddOrderMessage>(*message))
            {
                const auto& [order] = std::get<AddOrderMessage>(*message);
                std::cout << "Worker " << symbolId_ << ": Processing AddOrder ID " << order.GetOrderId() << std::endl;
                const Trades trades = orderBook_.AddOrder(std::make_shared<Order>(order));
                if (!trades.empty())
                {
                    std::cout << "Worker " << symbolId_ << ": Generated " << trades.size() << " trades:" << std::endl;
                    for (const auto& trade : trades)
                    {
                        std::cout << "  Trade: " << trade.GetBidTradeInfo().quantity
                                  << " shares at price " << trade.GetBidTradeInfo().price
                                  << " (Buy Order ID: " << trade.GetBidTradeInfo().orderId
                                  << ", Sell Order ID: " << trade.GetAskTradeInfo().orderId << ")" << std::endl;
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
                    std::cout << "Worker " << symbolId_ << ": Modify generated " << trades.size() << " trades" << std::endl;
                    for (const auto& trade : trades)
                    {
                        std::cout << "  Trade: " << trade.GetBidTradeInfo().quantity
                                  << " shares at price " << trade.GetBidTradeInfo().price
                                  << " (Buy Order ID: " << trade.GetBidTradeInfo().orderId
                                  << ", Sell Order ID: " << trade.GetAskTradeInfo().orderId << ")" << std::endl;
                    }
                }
            }
            else if (std::holds_alternative<CancelOrderMessage>(*message))
            {
                const auto& [OrderId] = std::get<CancelOrderMessage>(*message);
                std::cout << "Worker " << symbolId_ << ": Processing CancelOrder ID " << OrderId << std::endl;
                if (orderBook_.CancelOrder(OrderId))
                {
                    std::cout << "Worker " << symbolId_ << ": Order " << OrderId << " cancelled successfully" << std::endl;
                }
                else
                {
                    std::cout << "Worker " << symbolId_ << ": Failed to cancel order " << OrderId << std::endl;
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}