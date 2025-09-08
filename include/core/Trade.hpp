#pragma once
#include "Order.hpp"
#include <vector>

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