#include "OrderBook.h"

void OrderBook::reset() {
    bids.clear();
    asks.clear();
    orders.clear();
}

void OrderBook::addOrder(const Order &o) {
    orders[o.id] = o;
    if (o.side == 'B') {
        auto &lvl = bids[o.price];
        lvl.size  += o.size;
        lvl.count += 1;
    } else {
        auto &lvl = asks[o.price];
        lvl.size  += o.size;
        lvl.count += 1;
    }
}

void OrderBook::tradeCancel(const OrderID id, int qty) {
    auto it = orders.find(id);
    if (it == orders.end()) return;

    Order &o = it->second;
    if (qty >= o.size) {
        
        if (o.side == 'B') {
            auto lvlIt = bids.find(o.price);
            if (lvlIt != bids.end()) {
                lvlIt->second.size  -= o.size;
                lvlIt->second.count -= 1;
                if (lvlIt->second.count == 0)
                    bids.erase(lvlIt);
            }
        } else {
            auto lvlIt = asks.find(o.price);
            if (lvlIt != asks.end()) {
                lvlIt->second.size  -= o.size;
                lvlIt->second.count -= 1;
                if (lvlIt->second.count == 0)
                    asks.erase(lvlIt);
            }
        }
        orders.erase(it);
    } else {
        o.size -= qty;
        if (o.side == 'B') {
            auto lvlIt = bids.find(o.price);
            if (lvlIt != bids.end())
                lvlIt->second.size -= qty;
        } else {
            auto lvlIt = asks.find(o.price);
            if (lvlIt != asks.end())
                lvlIt->second.size -= qty;
        }
    }
}

std::vector<std::pair<double, Level>> OrderBook::topLevels(char side, int depth) const {
    std::vector<std::pair<double, Level>> out;

    if (side == 'B') {
        for (auto it = bids.begin(); it != bids.end() && (int)out.size() < depth; ++it)
            out.emplace_back(it->first, it->second);
    } else {
        for (auto it = asks.begin(); it != asks.end() && (int)out.size() < depth; ++it)
            out.emplace_back(it->first, it->second);
    }

    return out;
}
