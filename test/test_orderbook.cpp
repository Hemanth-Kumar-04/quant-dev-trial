#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "OrderBook.h"

TEST_CASE("Basic add and snapshot", "[orderbook]") {
    OrderBook ob;
    ob.reset();
    ob.addOrder({1, 10.0, 100, 'B'});
    ob.addOrder({2, 9.5, 200, 'B'});
    auto top = ob.topLevels('B', 2);
    REQUIRE(top.size()==2);
    REQUIRE(top[0].first==10.0);
    REQUIRE(top[1].first==9.5);
}

TEST_CASE("Trade reduces size and removes order", "[orderbook]") {
    OrderBook ob;
    ob.reset();
    ob.addOrder({1, 5.0, 50, 'A'});
    ob.tradeCancel(1, 20);
    auto top = ob.topLevels('A',1);
    REQUIRE(top[0].second.size==30);
    ob.tradeCancel(1, 30);
    REQUIRE(ob.topLevels('A',1).empty());
}