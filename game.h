#pragma once
#include <vector>
#include <unordered_set>
#include "game_width.h"
#include "tile_size.h"
#include "tile_exploration.h"
#include "input.h"
#include "assets.h"
#include "draw.h"
#include "item.h"
#include "marketplace.h"
#include "oasis.h"
#include "rocks.h"
#include "treasure.h"

#define CAPACITY_PER_CARAVANEER 25
#define WATER_PER_CARAVANEER 6

enum class GameState {
    READY,
    INTERSTITIAL,
    TRADE,
    OASIS_MSG,
    BANDITS,
    TREASURE,
    GAME_OVER
};

enum class MoveDirection {
    NONE,
    NORTH,
    WEST,
    SOUTH,
    EAST
};

struct Game {
    GameState state;
    int caravan_x;
    int caravan_y;
    int caravaneers;
    int waters;
    int money;
    Market* cur_market;
    float cam_x;
    float cam_y;
    int selector_x;
    int selector_y;

    // Only for messaging
    int stolen_bandit_money = 0;
    int treasured_gold = 0;
    std::vector<Item*> treasured_items;
    std::vector<int> treasured_qtys;

    // Lists
    std::vector<Item*> inventory; // indexes items. contains duplicates for qty
    std::vector<Market> markets;
    std::vector<Oasis> oases;
    std::vector<Rock> rocks;
    std::vector<Treasure> treasures;
    std::unordered_set<int> explored_tiles;
};

void init_game(Game& game, Assets& assets);
void update_game(Game& game, Input& input, Assets& assets, DrawContext& draw_context);
int food_count(Game& game);
int weight_capacity(Game& game);
int weight_current(Game& game);
void generate_market(Game& game, Assets& assets, MarketType type);
void generate_oasis(Game& game);
void random_position(int& out_x, int& out_y);
