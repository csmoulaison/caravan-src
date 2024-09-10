#include "turn.h"

void simulate_turn(Game& game, Assets& assets, MoveDirection move) {
    switch(move) {
    case MoveDirection::NORTH:
        if(game.caravan_y <= 0) {
            playSoundFromMemory(assets.nosale, VOLUME_MID);
            return;
        }
        game.caravan_y -= 1;
        break;
    case MoveDirection::WEST:
        if(game.caravan_x <= 0) {
            playSoundFromMemory(assets.nosale, VOLUME_MID);
            return;
        }
        game.caravan_x -= 1;
        break;
    case MoveDirection::SOUTH:
        if(game.caravan_y >= GAME_HEIGHT - 1) {
            playSoundFromMemory(assets.nosale, VOLUME_MID);
            return;
        }
        game.caravan_y += 1;
        break;
    case MoveDirection::EAST:
        if(game.caravan_x >= GAME_WIDTH - 1) {
            playSoundFromMemory(assets.nosale, VOLUME_MID);
            return;
        }
        game.caravan_x += 1;
        break;
    default:
        break;
    }

    bool ready_game_over = false; // only comes to fruition if ending in ready mode
    game.waters--;
    for(int i = 0; i < game.caravaneers; i++) {
        int cheapest_food = -1;
        for(int i = 0; i < game.inventory.size(); i++) {
            Item* item = game.inventory[i];

            if(item->type != ItemType::FOOD) {
                continue;
            }

            if(cheapest_food == -1 || item < game.inventory[cheapest_food]) {
                cheapest_food = i;
            }
        }
        if(cheapest_food != -1) { // if it does == -1, we are dead
            game.inventory.erase(game.inventory.begin() + cheapest_food);
        }

        if(game.waters < 0 || cheapest_food == -1) {
            ready_game_over = true;
        }
    }

    for(Market& market : game.markets) {
        if(market.x == game.caravan_x && market.y == game.caravan_y) {
            game.waters = game.caravaneers * WATER_PER_CARAVANEER;
            game.state = GameState::TRADE;
            game.cur_market = &market;
            playSoundFromMemory(assets.trade_open, VOLUME_MID);
        }

        for(Item& item : assets.items) {
            int market_count = 0;
            for(Item* market_item : market.inventory) {
                if(market_item == &item) {
                    market_count++;
                }
            }

            if(market_count > 0) {
                // item exists in market
                int difference = rand() % item.average_movement - item.average_movement / 2;
                difference++;
                if(difference > 0) {
                    // adding items
                    for(int i = 0; i < rand() % item.average_movement; i++) {
                        market.inventory.push_back(&item);
                    }
                } else {
                    // removing items
                    int removed = 0;
                    for(int i = 0; i < rand() % item.average_movement; i++) {
                        for(int j = 0; j < market.inventory.size(); j++) {
                            if(market.inventory[j] == &item) {
                                market.inventory.erase(market.inventory.begin() + j);
                                removed++;
                                if(-removed >= difference) {
                                    break;
                                }
                            }
                        }
                    }
                }
            } else {
                // item does not currently exist in market
                if(rand() % ((item.inherent_value + 10) / 10) == 0) {
                    for(int i = 0; i < rand() % item.average_movement; i++) {
                        market.inventory.push_back(&item);
                    }
                }
            }
        }
    }

    for(Oasis& oasis : game.oases) {
        if(oasis.x == game.caravan_x && oasis.y == game.caravan_y) {
            game.waters = game.caravaneers * WATER_PER_CARAVANEER;
            playSoundFromMemory(assets.oasis, VOLUME_MID);
            game.state = GameState::OASIS_MSG;
        }
    }

    if(game.state == GameState::READY) {
        if(ready_game_over) {
            game.caravaneers--;
            if(game.caravaneers < 1) {
                ready_game_over = true;
                game.state = GameState::GAME_OVER;
                playSoundFromMemory(assets.game_over, VOLUME_MID);
                return;
            }
        }

        if(!try_bandit(game, assets)) {   
            playSoundFromMemory(assets.step, VOLUME_MID);
        }
    }

    explore_tiles(game.explored_tiles, game.caravan_x, game.caravan_y);
}
