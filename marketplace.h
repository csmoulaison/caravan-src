#pragma once
#include <vector>
#include <string>
#include "item.h"

enum class MarketType {
    CITY,
    CARAVANSERAI
};

struct MarketPrice {
    Item* item;
    float demand;
    float supply;
};

struct Market {
    MarketType type;
    int x;
    int y;
    std::vector<MarketPrice> prices;
    std::vector<Item*> inventory;
    std::string* name;
};
