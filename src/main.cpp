#include "core/OrderBook.hpp"
#include "core/Router.hpp"
#include <iostream>

#include "core/SymbolWorker.hpp"

int main()
{
    Router router(1,100);

    SymbolWorker worker1(1, router.GetQueueForSymbol(1));

    worker1.Start();

    router.RouteOrder(Order(1,1,OrderType::GoodTillCancel,Side::Buy,100,10));
    router.RouteOrder(Order(1,2,OrderType::GoodTillCancel,Side::Sell,101,100));
    router.RouteOrder(Order(1,3,OrderType::GoodTillCancel,Side::Buy,101,3));

    // AddOrder(std::make_shared<Order>(1,1,OrderType::GoodTillCancel,Side::Buy,100,10));
    // ModifyOrder( OrderModify{1,Side::Buy,101,200} );
    // CancelOrder( 1 );

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // sleeping now for testing
    worker1.Stop();
    std::cout << "Test Completed" << std::endl;
    return 0;
}
