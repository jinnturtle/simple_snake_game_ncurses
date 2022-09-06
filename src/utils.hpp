#ifndef UTILS_HPP_
#define UTILS_HPP_

enum Keybind {
    KEYBIND_null  = '\0',
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

#endif // UTILS_HPP_
