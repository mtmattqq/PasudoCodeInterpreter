#ifndef COLOR_H
#define COLOR_H

#include <iostream>

#define RESET "\e[0m"

struct Color {
    short red, green, blue;
    Color();
    Color(int r, int g, int b);
};

std::ostream& operator<<(std::ostream &out, const Color &color);

#endif