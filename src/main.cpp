#include "core/OrderBook.hpp"

int main()
{
    OrderBook orderBook;
    orderBook.AddOrder(std::make_shared<Order>(1,OrderType::GoodTillCancel,Side::Buy,100,10));
    orderBook.AddOrder(std::make_shared<Order>(2,OrderType::GoodTillCancel,Side::Sell,101,100));
    orderBook.AddOrder(std::make_shared<Order>(3,OrderType::GoodTillCancel,Side::Buy,101,3));
    orderBook.ModifyOrder( OrderModify{1,Side::Buy,101,200} );
    orderBook.CancelOrder( 1 );
}