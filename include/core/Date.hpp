#pragma once

#include <iomanip>
#include <sstream>
#include <string>

namespace eulite {

struct GameDate {
    int year = 1444;
    int month = 11;
    int day = 11;

    void advanceMonth() {
        ++month;
        if (month > 12) {
            month = 1;
            ++year;
        }
    }

    std::string toString() const {
        std::ostringstream out;
        out << std::setfill('0') << std::setw(2) << day << '/'
            << std::setw(2) << month << '/' << year;
        return out.str();
    }
};

} // namespace eulite
