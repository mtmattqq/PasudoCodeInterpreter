/// --------------------
/// Position
/// --------------------

#ifndef POSITION_H
#define POSITION_H

#include <string>

struct Position {
    int index, line, column;
    std::string file_name;
    Position(int idx = 0, int ln = 0, int col = 0, const std::string& _file_name = "")
        : index(idx), line(ln), column(col), file_name(_file_name) {}
    void advance(char);
    std::string get_pos();
    friend std::ostream& operator<<(std::ostream &out, Position &pos);
};

#endif