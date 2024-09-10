#include <iostream>
#include <time.h>
#include "game.h"
#include "audio.h"

/*
TODO (CORE)
- Swords/bandits
- Travelling merchant
*/

int main(int argc, char** argv) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);

    initAudio();

    DrawContext draw_context;
    init_draw_context(draw_context);

    Input input;
    init_input(input);

    Assets assets;
    init_assets(assets);

    Game game;
    init_game(game, assets);

    while(!input.quit_application) {
        input_poll(input, draw_context.pixel_scalar);
        if(input.quit.held) {
            input.quit_application = true;
        }
        draw_clear_screen(draw_context);
        update_game(game, input, assets, draw_context);
        draw_present(draw_context);
    }

    return 0;
}
