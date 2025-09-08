#include "core/OrderBook.hpp"

const Price& OrderBook::GetBestBid() const
{
    //assuming bids is not empty
    return bids.begin()->first;
}

const Price& OrderBook::GetBestAsk() const
{
    //assuming asks is not empty
    return asks.begin()->first;
}

bool OrderBook::CanMatch(const Side& side,const Price& price) const
{
    if (Side::Buy == side)
    {
        if (asks.empty()) return false;
        //Get best ask and return price >= bestask
        return price >= GetBestAsk();
    }
    else
    {
        if (bids.empty()) return false;
        //Get best bid and return bestbid >= price
        return GetBestBid() >= price;
    }
}

Trades OrderBook::MatchOrders()
{
    Trades trades;
    trades.reserve(orders.size());

    while (true)
    {
        if ( bids.empty() || asks.empty() ) break;

        const Price BestBidPrice = GetBestBid();
        const Price BestAskPrice = GetBestAsk();

        if ( BestBidPrice < BestAskPrice ) break;

        auto& bidOrders = bids.begin()->second;
        auto& askOrders = asks.begin()->second;

        while (!bidOrders.empty() && !askOrders.empty())
        {
            const OrderPointer BidOrder = bidOrders.front();
            const OrderPointer AskOrder = askOrders.front();

            Quantity quantityToFill = std::min(BidOrder->GetRemainingQuantity(), AskOrder->GetRemainingQuantity());

            BidOrder->Fill(quantityToFill);
            AskOrder->Fill(quantityToFill);

            //if any Order is filled, remove that order from queue and remove from orders map

            if (BidOrder->IsFilled())
            {
                bidOrders.pop_front();
                orders.erase(BidOrder->GetOrderId());
            }

            if (AskOrder->IsFilled())
            {
                askOrders.pop_front();
                orders.erase(AskOrder->GetOrderId());
            }

            trades.emplace_back
            (
                TradeInfo {BidOrder->GetOrderId(),BidOrder->GetPrice(),quantityToFill},
                TradeInfo {AskOrder->GetOrderId(),AskOrder->GetPrice(),quantityToFill}
            );


        }

        if (bidOrders.empty())
        {
            //remove current price entry from bidsmap
            bids.erase(BestBidPrice);
        }

        if (askOrders.empty())
        {
            //remove current price entry from asksmap
            asks.erase(BestAskPrice);
        }
    }

    return trades;
}

Trades OrderBook::AddOrder(const OrderPointer& order)
{
    if (orders.contains(order->GetOrderId())) return {};

    OrderPointers::iterator iterator;

    if (order->GetSide() == Side::Buy)
    {
        auto& bidOrders = bids[order->GetPrice()];
        bidOrders.push_back(order);
        iterator = std::prev(bidOrders.end());
    }
    else
    {
        auto& askOrders = asks[order->GetPrice()];
        askOrders.push_back(order);
        iterator = std::prev(askOrders.end());
    }

    orders[order->GetOrderId()] = OrderEntry { order , iterator };

    return MatchOrders();
}

bool OrderBook::CancelOrder(const OrderId& orderId)
{
    if (!orders.contains(orderId)) return false;

    const auto& [order, iterator] = orders.at(orderId);

    const Price OrderPrice = order->GetPrice();

    if (order->GetSide() == Side::Buy)
    {
        auto& BidOrders = bids.at(OrderPrice);
        BidOrders.erase(iterator);
        if (BidOrders.empty())
            bids.erase(OrderPrice);
    }
    else
    {
        auto& AskOrders = asks.at(OrderPrice);
        AskOrders.erase(iterator);
        if (AskOrders.empty())
            asks.erase(OrderPrice);
    }

    orders.erase(order->GetOrderId());

    return true;
}

Trades OrderBook::ModifyOrder(const OrderModify& orderModify)
{
    if ( !orders.contains(orderModify.GetOrderId()) ) return {};

    const OrderType CurrentOrderType = orders.at(orderModify.GetOrderId()).order->GetOrderType();

    CancelOrder(orderModify.GetOrderId());

    return AddOrder( orderModify.ToOrderPointer(CurrentOrderType) );
}
