#pragma once

#include <string>
#include <vector>

namespace eulite {

struct Region {
    int id = 0;
    std::string name;
};

struct Province {
    int id = 0;
    std::string name;
    std::string ownerTag;
    int regionId = 0;
    double development = 1.0;
    std::string terrain = "plains";
    std::vector<int> adjacentProvinceIds;
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

    std::vector<Region>& regions() { return regions_; }
    const std::vector<Region>& regions() const { return regions_; }
    std::vector<Province>& provinces() { return provinces_; }
    const std::vector<Province>& provinces() const { return provinces_; }
    std::vector<Country>& countries() { return countries_; }
    const std::vector<Country>& countries() const { return countries_; }

    Country* findCountry(const std::string& tag);
    const Country* findCountry(const std::string& tag) const;
    const Region* findRegion(int id) const;
    const Province* findProvince(int id) const;
    bool areAdjacent(int provinceA, int provinceB) const;
    int countOwnedProvinces(const std::string& tag) const;
    double developmentOf(const std::string& tag) const;

private:
    bool loadUnchecked(const std::string& dataDirectory, std::string& error);

    std::vector<Region> regions_;
    std::vector<Province> provinces_;
    std::vector<Country> countries_;
};

} // namespace eulite
