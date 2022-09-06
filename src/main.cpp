#include <ncurses.h>
#include <thread>

#include "Game_data.hpp"
#include "Snake.hpp"
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
// displays the "game over" screen
auto game_over(Game_data* game) -> void;
// displays a welcome message and basic help info
auto start_screen() -> void;
// returns a string representing the program version, format <MAJ>.<MIN>.<FIX>
auto ver_str() -> std::string;

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
    Game_data game(Square{1, 2, 40, 20});
    Snake snake(&game);

    Keybind key {KEYBIND_null};
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
            default:
                break;
        }

        game.update();
        snake.update(&game);

        // TODO add game over screen
        if (snake.get_alive()) {
            game.score += snake.get_length();
        }

        ss_buf << "length: " << snake.get_length() << " score: " << game.score;

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

    game_over(&game);

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
    curs_set(0); // set cursor to become invisible
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

auto game_over(Game_data* game) -> void
{
    /* TODO implement a score board that can also be saved in the filesystem
     * and display it here
     */
    nodelay(stdscr, false);
    std::stringstream ss_buf;

    ss_buf
    << " ****************************************\n"
    << " **********     GAME OVER     ***********\n"
    << " ****************************************\n"
    << "                                         \n"
    << "  Final score: " << game->score << "\n"
    << "                                         \n"
    << " ****************************************";

    mvaddstr(10, 0, ss_buf.str().c_str());
    getch();
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
