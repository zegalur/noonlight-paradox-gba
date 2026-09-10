/*!****************************************************************************
 * @file main.cppm
 * @brief Program entry point.
 *******************************************************************************/        

import engine;
import game;

game::Data game_data;
using namespace gba;

int main() {
    run_subgame(game_data,
            game::init_game,
            game::update_game,
            game::draw_game);
    return 0;
}
