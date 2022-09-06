#ifndef SNAKE_HPP_
#define SNAKE_HPP_

#include <list>

#include "Game_data.hpp"
#include "utils.hpp"

class Snake final {
public:
    Snake(Game_data* game);

    auto get_alive() -> bool;
    auto get_length() -> size_t;

    auto grow(int ammount) -> void; // grows snake by additional ammount
    auto render() -> void;
    auto steer(Keybind key) -> void;
    auto update(Game_data* game) -> void;

private:
    auto check_self_collision(Vec2* pos) -> bool; // true if collided with self

    bool alive;
    Direction direction;
    size_t length;
    std::list<Vec2> segments;
};


#endif // SNAKE_HPP_
