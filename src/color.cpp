#include "color.h"

Color::Color() {red = green = blue = 0;}
Color::Color(int r, int g, int b) {
    red = r;
    green = g;
    blue = b;
}

std::ostream& operator<<(std::ostream &out, const Color &color) {
    out << "\e[38;2;" << color.red << ";" << color.green << ";" << color.blue << "m";
    return out;
}