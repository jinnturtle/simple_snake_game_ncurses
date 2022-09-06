#include "Game_data.hpp"

Game_data::Game_data(Square board)
: score {0}
, board {board}
, food {0, Vec2{0, 0}}
{
    int min_x = this->board.x;
    int max_x = this->board.x + this->board.w - 1;
    int min_y = this->board.y;
    int max_y = this->board.y + this->board.h - 1;

    std::random_device rd;
    this->rand_gen = std::mt19937(rd());
    this->distr_x = std::uniform_int_distribution<size_t>(min_x, max_x);
    this->distr_y = std::uniform_int_distribution<size_t>(min_y, max_y);
}

auto Game_data::get_board() -> Square
{
    return this->board;
}

auto Game_data::check_food_collision(Vec2* pos) -> bool
{
    if (this->food.pos.x == pos->x && this->food.pos.y == pos->y) {
        return true;
    }

    return false;
}

auto Game_data::check_out_of_bounds(Vec2* pos) -> bool
{
    if (pos->x < this->board.x || pos->x > this->board.x + this->board.w-1 ||
        pos->y < this->board.y || pos->y > this->board.y + this->board.h-1
    ) {
        return true;
    }

    return false;
}

auto Game_data::eat_food() -> int
{
    int nutrition = this->food.nutrition;
    this->food.nutrition = 0;
    return nutrition;
}

auto Game_data::render() -> void
{
    for (int x {this->board.x - 1}; x <= this->board.x + this->board.w; ++x) {
        mvaddch(this->board.y - 1            , x, '#');
        mvaddch(this->board.y + this->board.h, x, '#');
    }

    for (int y {this->board.y - 1}; y <= this->board.y + this->board.h; ++y) {
        mvaddch(y, this->board.x - 1            , '#');
        mvaddch(y, this->board.x + this->board.w, '#');
    }

    this->food.render();
}

auto Game_data::update() -> void
{
    if (this->food.nutrition == 0) {
        this->food.pos.x = distr_x(rand_gen);
        this->food.pos.y = distr_y(rand_gen);
        this->food.nutrition = 1;
    }
}
