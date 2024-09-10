#include "game.h"
#include <string>
#include <iostream>
#include "game_ready_state.h"
#include "game_over_state.h"
#include "handle_game_trade.h"

#define MIN_DIST_MARKETS 8
#define MIN_DIST_OASES 6
#define DEBUG_EXPLORE_ALL false

void init_game(Game& game, Assets& assets) {
    game.state = GameState::READY;
    random_position(game.caravan_x, game.caravan_y);
    game.caravaneers = 1;
    game.waters = game.caravaneers * WATER_PER_CARAVANEER;
    game.money = 100;
    game.cur_market = nullptr;
    game.cam_x = -112;
    game.cam_y = 0;
    game.selector_x = 0;
    game.selector_y = 0;
    game.stolen_bandit_money = 0;

    game.inventory = {
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[0],
        &assets.items[2],
        &assets.items[2],
    };

    game.markets.clear();
    generate_market(game, assets, MarketType::CITY);
    generate_market(game, assets, MarketType::CITY);
    generate_market(game, assets, MarketType::CITY);
    generate_market(game, assets, MarketType::CITY);
    generate_market(game, assets, MarketType::CITY);
    generate_market(game, assets, MarketType::CARAVANSERAI);
    generate_market(game, assets, MarketType::CARAVANSERAI);
    generate_market(game, assets, MarketType::CARAVANSERAI);
    generate_market(game, assets, MarketType::CARAVANSERAI);
    generate_market(game, assets, MarketType::CARAVANSERAI);
    generate_market(game, assets, MarketType::CARAVANSERAI);

    game.oases.clear();
    generate_oasis(game);
    generate_oasis(game);
    generate_oasis(game);
    generate_oasis(game);
    generate_oasis(game);
    generate_oasis(game);
    generate_oasis(game);
    generate_oasis(game);

    game.rocks.clear();

    game.treasures.clear();
    for(int i = 0; i < 12; i++) {
        // BULLSHIT: it's called a rock because it used to be a rock and this is a game jam.
        Treasure rock;
        //rock.sprite = &assets.sprite_details[rand() % (assets.sprite_details.size() - 1)];

        bool eligible_position = true;
        int min_dist = 2;
        int min_rock_dist = 4;
        for(int j = 0; j < 1000; j++) {
            eligible_position = true;
            random_position(rock.x, rock.y);

            for(Rock& other : game.rocks) {
                if(abs(other.x - rock.x) + abs(other.y - rock.y) < min_rock_dist) {
                    eligible_position = false;
                    break;
                }
            }
            for(Market& other : game.markets) {
                if(abs(other.x - rock.x) + abs(other.y - rock.y) < min_dist) {
                    eligible_position = false;
                    break;
                }
            }
            for(Oasis& other : game.oases) {
                if(abs(other.x - rock.x) + abs(other.y - rock.y) < min_dist) {
                    eligible_position = false;
                    break;
                }
            }

            if(eligible_position) {
                break;
            }
        }

        if(eligible_position) {
            game.treasures.push_back(rock);
        }
    }

    game.explored_tiles.clear();
    explore_tiles(game.explored_tiles, game.caravan_x, game.caravan_y);
    if(DEBUG_EXPLORE_ALL) {
        for(int i = 0; i < GAME_WIDTH * GAME_HEIGHT; i++) {
            game.explored_tiles.insert(i);
        }
    }
}

void update_game(Game& game, Input& input, Assets& assets, DrawContext& draw_context) {
    bool bang_reset = false;
    int curx;
    int cury;

    switch(game.state) {
    case GameState::READY:
        handle_game_ready(game, assets, input);
        draw_set_bg_tan(draw_context);
        draw_game_ready(game, input, assets, draw_context);
        draw_inventory_ui(draw_context, input, game.inventory, 0);
        draw_player_header_ui(game, assets, draw_context);
        break;
    case GameState::TRADE:
        draw_game_trade(game, input, assets, draw_context);
        break;
    case GameState::OASIS_MSG:
        if(input.select.just_pressed) {
            game.state = GameState::READY;
            playSoundFromMemory(assets.step, VOLUME_MID);
        }
        draw_set_bg_tan(draw_context);
        draw_game_ready(game, input, assets, draw_context);
        draw_inventory_ui(draw_context, input, game.inventory, 0);
        draw_player_header_ui(game, assets, draw_context);

        draw_dialog(draw_context, DialogStyle::SOFT, 144, 80, 164, 19);
        draw_text(draw_context, "FOUND AN OASIS", Font::BIG, 148, 82, nullptr);
        draw_text(draw_context, "YOUR WATER SUPPLY HAS BEEN REPLENISHED", Font::SMALL, 148, 91, nullptr);
        break;
    case GameState::BANDITS:
        if(input.select.just_pressed) {
            game.state = GameState::READY;
            playSoundFromMemory(assets.step, VOLUME_MID);
        }
        draw_set_bg_tan(draw_context);
        draw_game_ready(game, input, assets, draw_context);
        draw_inventory_ui(draw_context, input, game.inventory, 0);
        draw_player_header_ui(game, assets, draw_context);
        draw_dialog(draw_context, DialogStyle::SOFT, 144, 80, 164, 25);
        draw_text(draw_context, "BANDIT ATTACK", Font::BIG, 148, 82, nullptr);
        draw_text(draw_context, "YOU HAVE BEEN ATTACKED BY BANDITS", Font::SMALL, 148, 91, nullptr);
        curx = 148;
        draw_text(draw_context, "THEY STOLE ", Font::SMALL, curx, 97, &curx);
        draw_text(draw_context, std::to_string(game.stolen_bandit_money), Font::SMALL, curx, 97, &curx);
        draw_text(draw_context, " COINS", Font::SMALL, curx, 97, &curx);
        break;
    case GameState::TREASURE:
        if(input.select.just_pressed) {
            game.state = GameState::READY;
            playSoundFromMemory(assets.step, VOLUME_MID);
        }
        draw_set_bg_tan(draw_context);
        draw_game_ready(game, input, assets, draw_context);
        draw_inventory_ui(draw_context, input, game.inventory, 0);
        draw_player_header_ui(game, assets, draw_context);

        draw_dialog(draw_context, DialogStyle::SOFT, 144, 80, 164, 19);
        draw_text(draw_context, "FOUND A TREASURE CHEST", Font::BIG, 148, 82, nullptr);
        curx = 148;
        draw_text(draw_context, "YOU RETRIEVED ", Font::SMALL, curx, 91, &curx);
        draw_text(draw_context, std::to_string(game.treasured_gold), Font::SMALL, curx, 91, &curx);
        draw_text(draw_context, " GOLD", Font::SMALL, curx, 91, &curx);
        break;
    case GameState::GAME_OVER:
        if(game.waters < 0) {
            game.waters = 0;
        }

        if(input.select.just_pressed) {
            init_game(game, assets);
            game.state = GameState::READY;
            playSoundFromMemory(assets.step, VOLUME_MID);
        }
        draw_set_bg_tan(draw_context);
        draw_game_ready(game, input, assets, draw_context);
        draw_inventory_ui(draw_context, input, game.inventory, 0);
        draw_player_header_ui(game, assets, draw_context);

        draw_dialog(draw_context, DialogStyle::SOFT, 144, 80, 64, 13);
        draw_text(draw_context, "GAME OVER", Font::BIG, 148, 82, nullptr);
        break;
    default:
        break;
    }
}

int food_count(Game& game) {
    int count = 0;
    for(Item* item : game.inventory) {
        if(item->type == ItemType::FOOD) {
            count++;
        }
    }
    return count;
}

int weight_capacity(Game& game) {
    return game.caravaneers * CAPACITY_PER_CARAVANEER;
}

int weight_current(Game& game) {
    int weight = 0;
    for(Item* item : game.inventory) {
        weight += item->weight;
    }
    return weight;
}

void generate_market(Game& game, Assets& assets, MarketType type) {
    Market market;
    market.type = type;

    int iterations = 250;
    int min_dist = MIN_DIST_MARKETS;
    for(int i = 0; i < iterations; i++) {
        if(i % 40 == 0) {
            min_dist -= 1;
        }

        random_position(market.x, market.y);
        bool eligible_position = true;
        for(Market& other : game.markets) {
            if(abs(other.x - market.x) + abs(other.y - market.y) < min_dist) {
                eligible_position = false;
            }
        }

        if(abs(market.x - GAME_WIDTH / 2) + abs(market.y - GAME_WIDTH / 2) > rand() % GAME_WIDTH * 0.75) {
            eligible_position = false;
        }

        if(eligible_position) {
            break;   
        }
    }


    for(Item& item : assets.items) {
        market.prices.push_back({
            &item,
            rand() / (float)RAND_MAX,
            rand() / (float)RAND_MAX
        });

        if(market.prices[market.prices.size() - 1].supply > 0.33) {
            int qty = 0;
            for(int i = 0; i < (rand() % 20) * market.prices[market.prices.size() - 1].supply; i++) {
                qty += rand() % item.average_movement;
            }

            for(int i = 0; i < qty; i++) {
                market.inventory.push_back(&item);
            }
        }
    }

    market.name = &assets.market_names[rand() % assets.market_names.size() - 1];
    game.markets.push_back(market);
}

void generate_oasis(Game& game) {
    Oasis oasis;

    int iterations = 5000;
    int min_dist = MIN_DIST_OASES;
    for(int i = 0; i < iterations; i++) {
        if(i % 2000 == 0) {
            min_dist -= 1;
        }

        random_position(oasis.x, oasis.y);
        bool eligible_position = true;
        for(Oasis& other : game.oases) {
            if(abs(other.x - oasis.x) + abs(other.y - oasis.y) < min_dist) {
                eligible_position = false;
            }
        }
        for(Market& other : game.markets) {
            if(abs(other.x - oasis.x) + abs(other.y - oasis.y) < min_dist) {
                eligible_position = false;
            }
        }

        if(abs(oasis.x - GAME_WIDTH / 2) + abs(oasis.y - GAME_WIDTH / 2) > rand() % GAME_WIDTH * 0.75) {
            eligible_position = false;
        }

        if(eligible_position) {
            break;   
        }
    }

    game.oases.push_back(oasis);
}

void random_position(int& out_x, int& out_y) {
    out_x = rand() % GAME_WIDTH;
    out_y = rand() % GAME_HEIGHT;
}
