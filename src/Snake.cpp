#include "Snake.hpp"

Snake::Snake(Game_data* game)
: alive {true}
, direction {DIR_up}
 
{
    int start_x = game->get_board().x + game->get_board().w / 2;
    int start_y = game->get_board().y + game->get_board().h - 2;

    this->segments.push_back(Vec2{start_x, start_y});

    this->length = this->segments.size();
}

auto Snake::get_alive() -> bool
{
    return this->alive;
}

auto Snake::get_length() -> size_t
{
    return this->length;
}

auto Snake::grow(int ammount) -> void
{
    this->length += ammount;
}

auto Snake::render() -> void
{
    for (
        std::list<Vec2>::iterator i {this->segments.begin()};
        i != this->segments.end();
        ++i
    ) {
        char graphic {'*'};
        if (i == this->segments.begin()) {
            graphic = '@';
        }

        mvaddch(i->y, i->x, graphic);
    }
}

auto Snake::steer(Keybind key) -> void
{
    switch (key) {
        case KEYBIND_up:
            if (this->direction != DIR_down) { this->direction = DIR_up; }
            break;
        case KEYBIND_down:
            if (this->direction != DIR_up) { this->direction = DIR_down; }
            break;
        case KEYBIND_left:
            if (this->direction != DIR_right) { this->direction = DIR_left; }
            break;
        case KEYBIND_right:
            if (this->direction != DIR_left) { this->direction = DIR_right; }
            break;
        default:
            break;
    }
}

auto Snake::update(Game_data* game) -> void
{
    Vec2 pos {this->segments.front()};

    switch (this->direction) {
        case DIR_up:
            --pos.y;
            this->segments.push_front(pos);
            break;
        case DIR_down:
            ++pos.y;
            this->segments.push_front(pos);
            break;
        case DIR_left:
            --pos.x;
            this->segments.push_front(pos);
            break;
        case DIR_right:
            ++pos.x;
            this->segments.push_front(pos);
            break;
        default:
            break;
    }

    if (this->check_self_collision(&pos)) {
        this->alive = false;
        return;
    }

    if (game->check_out_of_bounds(&pos)) {
        this->alive = false;
        return;
    }

    if (game->check_food_collision(&pos)) {
        this->grow(game->eat_food());
    }

    /* snake might be shorter than it should be (e.g. if ate recently)
    * here we let it grow it it should*/
    if (this->segments.size() > this->length) {
        this->segments.pop_back();
    }
}

auto Snake::check_self_collision(Vec2* pos) -> bool
{
    for (
        std::list<Vec2>::iterator i {this->segments.begin()};
        i != this->segments.end();
        ++i
    ) {
        // skipping the head, can't bite your own head
        if (i == this->segments.begin()) { continue; }

        if (pos->x == i->x && pos->y == i->y) {
            return true;
        }
    }

    return false;
}
