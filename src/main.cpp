#include <cstdint>
#include <format>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

enum class Side: uint8_t { Buy = 0, Sell = 1 };
enum class OrderType: uint8_t { GoodTillCancel = 0, FillOrKill = 1 };

using Price = std::int64_t;
using Quantity = std::uint64_t;
using OrderId = std::uint64_t;

//Our Order Book, consists of Orders at specific price points, order priority-wise based on time they were placed.
//To handle this, our OrderBook will contain
//Two ordered Maps which will get PriceLevelInfo for specific price and can return top bids/asks
//<Price,Orders, std::greater> bids;
//<Price,Orders, std::less> asks;

class Order
{
public:
    Order(const OrderId& orderId, const OrderType& orderType , const Side& side, const Price& price, const Quantity& quantity):
        orderId{orderId},
        orderType{orderType},
        side{side},
        price{price},
        initialQuantity{quantity},
        remainingQuantity{quantity}
    {}

    const OrderId& GetOrderId() const { return orderId; }
    const OrderType& GetOrderType() const { return orderType; }
    const Side& GetSide() const { return side; }
    const Price& GetPrice() const { return price; }
    const Quantity& GetInitialQuantity() const { return initialQuantity; }
    const Quantity& GetRemainingQuantity() const { return remainingQuantity; }
    const Quantity GetFilledQuantity() const { return initialQuantity - remainingQuantity; }

    bool IsFilled() const { return GetRemainingQuantity() == 0; }

    void Fill(const Quantity& fillQuantity)
    {
        if (fillQuantity > GetRemainingQuantity())
        {
            throw std::logic_error(std::format("Order ({}) cannot be filled. Fill amount greater than remaining quantity",GetOrderId()));
        }

        remainingQuantity -= fillQuantity;
    }
private:
    OrderId orderId;
    OrderType orderType;
    Side side;
    Price price;
    Quantity initialQuantity;
    Quantity remainingQuantity;

};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

class OrderModify
{
public:
    OrderModify( const OrderId& orderId, const Side& side, const Price& price, const Quantity& quantity):
        orderId{orderId},
        side(side),
        price(price),
        quantity(quantity)
    { }

    const OrderId& GetOrderId() const { return orderId; }
    const Side& GetSide() const { return side; }
    const Price& GetPrice() const { return price; }
    const Quantity& GetQuantity() const { return quantity; }

    OrderPointer ToOrderPointer(OrderType type)
    {
        return std::make_shared<Order>(orderId, type, side, price, quantity);
    }
private:
    OrderId orderId;
    Side side;
    Price price;
    Quantity quantity;
};


struct TradeInfo
{
    OrderId orderId;
    Price price;
    Quantity quantity;
};

class Trade
{
public:
    Trade(const TradeInfo& bidTradeInfo, const TradeInfo& askTradeInfo):
        bidTradeInfo{bidTradeInfo},
        askTradeInfo{askTradeInfo}
    { }

    const TradeInfo& GetBidTradeInfo() const {return bidTradeInfo;}
    const TradeInfo& GetAskTradeInfo() const {return askTradeInfo;}
private:
    TradeInfo bidTradeInfo;
    TradeInfo askTradeInfo;
};

using Trades = std::vector<Trade>;

class OrderBook
{
    struct OrderEntry
    {
        OrderPointer order { nullptr };
        OrderPointers::iterator location {};
    };

    std::map<Price, OrderPointers, std::greater<> > bids; // Best bid
    std::map<Price, OrderPointers, std::less<> > asks; // Best ask
    std::unordered_map<OrderId, OrderEntry> orders;

    const Price& GetBestBid() const
    {
        //assuming bids is not empty
        return bids.begin()->first;
    }

    const Price& GetBestAsk() const
    {
        //assuming asks is not empty
        return asks.begin()->first;
    }

    bool CanMatch(const Side& side,const Price& price) const
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

    Trades MatchOrders()
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

public:
    Trades AddOrder(const OrderPointer& order)
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

    bool CancelOrder(const OrderId& orderId)
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

    Trades ModifyOrder(OrderModify& orderModify)
    {
        if ( !orders.contains(orderModify.GetOrderId()) ) return {};

        const OrderType CurrentOrderType = orders.at(orderModify.GetOrderId()).order->GetOrderType();

        CancelOrder(orderModify.GetOrderId());

        return AddOrder( orderModify.ToOrderPointer(CurrentOrderType) );
    }
};


int main()
{
    OrderBook orderBook;

    orderBook.AddOrder(std::make_shared<Order>(1,OrderType::GoodTillCancel,Side::Buy,100,10));
    orderBook.AddOrder(std::make_shared<Order>(2,OrderType::GoodTillCancel,Side::Sell,101,100));
    orderBook.AddOrder(std::make_shared<Order>(3,OrderType::GoodTillCancel,Side::Buy,101,3));
    OrderModify orderModify{1,Side::Buy,101,200};
    orderBook.ModifyOrder( orderModify );
    orderBook.CancelOrder( 1 );
}