#include <sstream>
#include <list>
#include <chrono>
#include <thread>

#include <ncurses.h>

#include "logs.hpp"

#define PROG_NAME "Snake"
#define PROG_V_MAJ 0
#define PROG_V_MIN 1
#define PROG_V_FIX 0

auto deinit_ncurses() -> void;
auto init_ncurses() -> void;

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

struct Vec2 {
    int x;
    int y;
};

class Snake final {
public:
    Snake();

    auto render() -> void;
    auto steer(Keybind key) -> void;
    auto update() -> void;
private:
    auto move(Vec2* pos) -> void; // move head to this position

    Direction direction;
    std::list<Vec2> segments;
};

Snake::Snake()
: direction {DIR_right}
{
    this->segments.push_back(Vec2{10, 10});
    this->segments.push_back(Vec2{10, 11});
    this->segments.push_back(Vec2{10, 12});
    this->segments.push_back(Vec2{10, 13});
    this->segments.push_back(Vec2{10, 14});
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

auto Snake::update() -> void
{
    Vec2 pos {this->segments.front()};

    switch (this->direction) {
        case DIR_up:
            --pos.y;
            this->move(&pos);
            break;
        case DIR_down:
            ++pos.y;
            this->move(&pos);
            break;
        case DIR_left:
            --pos.x;
            this->move(&pos);
            break;
        case DIR_right:
            ++pos.x;
            this->move(&pos);
            break;
        default:
            break;
    }
}

// moves the snake's head to this position and removes last element of tail
auto Snake::move(Vec2* pos) -> void
{
    this->segments.pop_back();
    this->segments.push_front(*pos);
}

auto main() -> int
{
    init_ncurses();

    start_screen();

    std::stringstream ss_buf;

    /* will be handling frame length via other means to have more control over
     * game speed */
    nodelay(stdscr, true);

    int game_speed {2};
    Snake snake;

    Keybind key {KEYBIND_up};
    while (key != KEYBIND_quit) {
        clear();
        snake.render();

        key = static_cast<Keybind>(getch());

        switch (key) {
            case KEYBIND_quit:
                continue;
            case KEYBIND_up:
            case KEYBIND_down:
            case KEYBIND_left:
            case KEYBIND_right:
                snake.steer(key);
        }

        snake.update();

        refresh();
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
    DBG(8, "init ncurses");

    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
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
