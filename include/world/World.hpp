#pragma once

#include <string>
#include <vector>

namespace eulite {

struct Province {
    int id = 0;
    std::string name;
    std::string ownerTag;
    double development = 1.0;
    std::string terrain = "plains";
};

struct Country {
    std::string tag;
    std::string name;
    double treasury = 0.0;
    int manpower = 0;
    double taxModifier = 1.0;
};

class World {
public:
    bool load(const std::string& dataDirectory, std::string& error);

    std::vector<Province>& provinces() { return provinces_; }
    const std::vector<Province>& provinces() const { return provinces_; }
    std::vector<Country>& countries() { return countries_; }
    const std::vector<Country>& countries() const { return countries_; }

    Country* findCountry(const std::string& tag);
    const Country* findCountry(const std::string& tag) const;
    int countOwnedProvinces(const std::string& tag) const;
    double developmentOf(const std::string& tag) const;

private:
    std::vector<Province> provinces_;
    std::vector<Country> countries_;
};

} // namespace eulite
