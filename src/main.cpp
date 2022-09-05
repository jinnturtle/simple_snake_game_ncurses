#include <sstream>
#include <vector>
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

enum {
    KEYBIND_up    = 'k',
    KEYBIND_down  = 'j',
    KEYBIND_left  = 'l',
    KEYBIND_right = 'h',
    KEYBIND_quit  = 'q'
};

struct vec2 {
    int x;
    int y;
};

class Snake final {
public:
    Snake();

    auto render() -> void;
private:
    std::vector<vec2> segments;
};

Snake::Snake()
{
    this->segments.push_back(vec2{10, 10});
    this->segments.push_back(vec2{10, 11});
}

auto Snake::render() -> void
{
    for (size_t i {0}; i < this->segments.size(); ++i) {
        char graphic {'*'};
        if (i == 0) {
            graphic = '@';
        }

        mvaddch(this->segments[i].y, this->segments[i].x, graphic);
    }
}

auto main(int argc, char** argv) -> int
{
    init_ncurses();

    start_screen();

    std::stringstream ss_buf;

    /* will be handling frame length via other means to have more control over
     * game speed */
    nodelay(stdscr, true);

    int game_speed {2};
    Snake snake;

    char ch {0};
    while (ch != KEYBIND_quit) {
        clear();
        snake.render();

        ch = getch();

        switch (ch) {
            case KEYBIND_quit:
                continue;
        }

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
