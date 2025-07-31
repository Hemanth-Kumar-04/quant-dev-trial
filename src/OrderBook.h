#pragma once
#include <map>
#include <unordered_map>
#include <vector>
#include <string>

typedef long long OrderID;

struct Order {
    OrderID id;
    double price;
    int size;
    char side; 
};

struct Level {
    int size = 0;
    int count = 0;
};

class OrderBook {
public:
    void reset();
    void addOrder(const Order &o);
    void tradeCancel(const OrderID id, int qty);
    std::vector<std::pair<double, Level>> topLevels(char side, int depth=10) const;
private:
    std::map<double, Level, std::greater<double>> bids;
    std::map<double, Level> asks;
    std::unordered_map<OrderID, Order> orders;
};