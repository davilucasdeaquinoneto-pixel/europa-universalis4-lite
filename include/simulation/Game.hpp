#pragma once

#include "core/Date.hpp"
#include "world/World.hpp"

#include <string>

namespace eulite {

class Game {
public:
    bool start(const std::string& dataDirectory, std::string& error);
    void advanceMonths(int count);
    void printStatus() const;
    void printProvinces() const;
    bool save(const std::string& path, std::string& error) const;

    const GameDate& date() const { return date_; }
    const World& world() const { return world_; }

private:
    void processMonthlyEconomy();

    GameDate date_;
    World world_;
    bool started_ = false;
};

} // namespace eulite
