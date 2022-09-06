#ifndef FOOD_HPP_
#define FOOD_HPP_

#include <ncurses.h>

#include "utils.hpp"

struct Food final {
    Food(int nutrition, Vec2 pos);

    auto render() -> void;

    int nutrition;
    Vec2 pos;
};

#endif // FOOD_HPP_
