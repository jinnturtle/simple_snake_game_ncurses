#include <sstream>
#include <list>
#include <chrono>
#include <thread>
#include <random>

#include <ncurses.h>

#include "logs.hpp"

#define PROG_NAME "Snake"
#define PROG_V_MAJ 0
#define PROG_V_MIN 1
#define PROG_V_FIX 0

#define BOARD_W 40
#define BOARD_H 20

auto deinit_ncurses() -> void;
auto init_ncurses() -> void;

// checks required hardware parameters, returns non 0 if requirements not met
auto check_hardware_req() -> int;
// displays a welcome message and basic help info
auto start_screen() -> void;
// returns a string representing the program version, format <MAJ>.<MIN>.<FIX>
auto ver_str() -> std::string;

enum Keybind {
    KEYBIND_up    = 'k',
    KEYBIND_down  = 'j',
    KEYBIND_left  = 'h',
    KEYBIND_right = 'l',
    KEYBIND_quit  = 'q'
};

enum Direction {
    DIR_up = 0,
    DIR_down,
    DIR_left,
    DIR_right
};

struct Square {
    int x;
    int y;
    int w;
    int h;
};

struct Vec2 {
    int x;
    int y;
};

struct Food final {
    Food(int nutrition, Vec2 pos);

    auto render() -> void;

    int nutrition;
    Vec2 pos;
};

Food::Food(int nutrition, Vec2 pos)
: nutrition {nutrition}
, pos {pos}
{}

auto Food::render() -> void
{
    mvaddch(this->pos.y, this->pos.x, '.');
}

class Game_data final {
public:
    Game_data(Square board);

    size_t score;

    auto check_food_collision(Vec2* pos) -> bool; // returns true on collision
    auto check_out_of_bounds(Vec2* pos) -> bool; // returns true if pos o.o.b.
    auto eat_food() -> int; // returns food nutrition and consumes food

    auto render() -> void;
    auto update() -> void;
private:
    Square board;
    Food food;

    std::mt19937 rand_gen;
    std::uniform_int_distribution<size_t> distr_x;
    std::uniform_int_distribution<size_t> distr_y;
};

Game_data::Game_data(Square board)
: score {0}
, board {board}
, food {0, Vec2{0, 0}}
{
    int min_x = this->board.x;
    int max_x = this->board.x + this->board.w;
    int min_y = this->board.y;
    int max_y = this->board.y + this->board.h;

    std::random_device rd;
    this->rand_gen = std::mt19937(rd());
    this->distr_x = std::uniform_int_distribution<size_t>(min_x, max_x);
    this->distr_y = std::uniform_int_distribution<size_t>(min_y, max_y);
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

class Snake final {
public:
    Snake();

    auto get_alive() -> bool;
    auto get_length() -> size_t;

    auto grow(int ammount) -> void; // grows snake by additional ammount
    auto render() -> void;
    auto steer(Keybind key) -> void;
    auto update(Game_data* game) -> void;

private:
    auto move(Vec2* pos) -> void; // move head to this position

    bool alive;
    Direction direction;
    size_t length;
    std::list<Vec2> segments;
};

Snake::Snake()
: alive {true}
, direction {DIR_right}
 
{
    this->segments.push_back(Vec2{10, 10});
    this->segments.push_back(Vec2{10, 11});

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
            this->direction = DIR_up;
            break;
        case KEYBIND_down:
            this->direction = DIR_down;
            break;
        case KEYBIND_left:
            this->direction = DIR_left;
            break;
        case KEYBIND_right:
            this->direction = DIR_right;
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

auto main() -> int
{
    init_ncurses();

    if (check_hardware_req()) {
        deinit_ncurses();
        logs::err("larger terminal is needed to run program");
        return 1;
    }
    start_screen();

    std::stringstream ss_buf;

    /* will be handling frame length via other means to have more control over
     * game speed */
    nodelay(stdscr, true);

    bool quit {false};
    int game_speed {2};
    size_t game_score {0};
    Game_data game(Square{1, 2, 40, 20});
    Snake snake;

    Keybind key {KEYBIND_up};
    while (!quit && snake.get_alive()) {
        clear();

        key = static_cast<Keybind>(getch());

        switch (key) {
            case KEYBIND_quit:
                quit = true;
                continue;
            case KEYBIND_up:
            case KEYBIND_down:
            case KEYBIND_left:
            case KEYBIND_right:
                snake.steer(key);
        }

        // TODO set playground boundaries (and maybe draw them on screen too)
        game.update();
        snake.update(&game);

        // TODO add game over screen
        if (snake.get_alive()) {
            game_score += snake.get_length();
        }

        ss_buf << "length: " << snake.get_length() << " score: " << game_score;

        game.render();
        snake.render();
        mvaddstr(0, 0, ss_buf.str().c_str());

        ss_buf.str(std::string());
        refresh();

        // TODO gradually increase game speed
        // TODO account for things taking up frame time
        std::this_thread::sleep_for(
            std::chrono::duration<double, std::milli>(1000 / game_speed)
        );
    }

    deinit_ncurses();

    return 0;
}

auto deinit_ncurses() -> void
{
    endwin();
}

auto init_ncurses() -> void
{
    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
}

auto check_hardware_req() -> int
{
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    int need_max_x = BOARD_W + 2; // +2 for boarders
    int need_max_y = BOARD_H + 3; // +2 for boarders, +1 game stats line
    if (max_x < need_max_x || max_y < need_max_y) {
        std::stringstream ss_buf;
        ss_buf
        << "need larger terminal!\n"
        << "min x: " << need_max_x << " (current: " << max_x << ")\n"
        << "min y: " << need_max_y << " (current: " << max_y << ")\n"
        << "press any key...";

        mvaddstr(0, 0, ss_buf.str().c_str());
        getch();

        return 1;
    }

    return 0;
}

auto start_screen() -> void
{
    std::stringstream ss_buf;

    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    ss_buf
    << "Hello, welcome to " << PROG_NAME << " v" << ver_str() << "\n"
    << "\n"
    << "CONTROLLS\n"
    << "h - move left\n"
    << "j - move down\n"
    << "k - move up\n"
    << "l - move right\n"
    << "q - quit game";

    mvaddstr(0, 0, ss_buf.str().c_str());
    mvaddstr(max_y - 1, 0, "press any key...");
    getch();
}

auto ver_str() -> std::string
{
    std::stringstream ss_buf;
    ss_buf << PROG_V_MAJ << "." << PROG_V_MIN << "." << PROG_V_FIX;
    return ss_buf.str();
}
