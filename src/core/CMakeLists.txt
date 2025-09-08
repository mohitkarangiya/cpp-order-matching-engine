
add_library(CoreLib STATIC
    Order.cpp
    OrderBook.cpp
)

target_include_directories(CoreLib PUBLIC ${PROJECT_INCLUDE_DIR})