#ifndef GAME_DATA_HPP_
#define GAME_DATA_HPP_

#include <random>

#include "Food.hpp"
#include "utils.hpp"

class Game_data final {
public:
    Game_data(Square board);

    size_t score;

    auto get_board() -> Square;

    auto check_food_collision(Vec2* pos) -> bool; // returns true on collision
    auto check_out_of_bounds(Vec2* pos) -> bool; // returns true if pos o.o.b.
    auto eat_food() -> int; // returns food nutrition and consumes food
                            //
    auto render() -> void;
    auto update() -> void;
private:
    Square board;
    Food food;

    std::mt19937 rand_gen;
    std::uniform_int_distribution<size_t> distr_x;
    std::uniform_int_distribution<size_t> distr_y;
};

#endif //GAME_DATA_HPP_
