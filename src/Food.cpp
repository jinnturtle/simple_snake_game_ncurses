#include "Food.hpp"

Food::Food(int nutrition, Vec2 pos)
: nutrition {nutrition}
, pos {pos}
{}

auto Food::render() -> void
{
    mvaddch(this->pos.y, this->pos.x, '.');
}
