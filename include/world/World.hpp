#pragma once

#include <string>
#include <vector>

namespace eulite {

struct Continent {
    int id = 0;
    std::string name;
};

struct Region {
    int id = 0;
    std::string name;
    int continentId = 0;
};

struct Subregion {
    int id = 0;
    std::string name;
    int regionId = 0;
};

struct Area {
    int id = 0;
    std::string name;
    int subregionId = 0;
};

enum class ConnectionType {
    Land,
    Sea,
    Strait,
    River,
    Impassable
};

struct ProvinceConnection {
    int provinceA = 0;
    int provinceB = 0;
    ConnectionType type = ConnectionType::Land;
    bool oneWay = false;
};

struct Province {
    int id = 0;
    std::string name;
    std::string ownerTag;
    int regionId = 0;
    int areaId = 0;
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

    std::vector<Continent>& continents() { return continents_; }
    const std::vector<Continent>& continents() const { return continents_; }
    std::vector<Region>& regions() { return regions_; }
    const std::vector<Region>& regions() const { return regions_; }
    std::vector<Subregion>& subregions() { return subregions_; }
    const std::vector<Subregion>& subregions() const { return subregions_; }
    std::vector<Area>& areas() { return areas_; }
    const std::vector<Area>& areas() const { return areas_; }
    std::vector<Province>& provinces() { return provinces_; }
    const std::vector<Province>& provinces() const { return provinces_; }
    std::vector<ProvinceConnection>& connections() { return connections_; }
    const std::vector<ProvinceConnection>& connections() const { return connections_; }
    std::vector<Country>& countries() { return countries_; }
    const std::vector<Country>& countries() const { return countries_; }

    Country* findCountry(const std::string& tag);
    const Country* findCountry(const std::string& tag) const;
    const Continent* findContinent(int id) const;
    const Region* findRegion(int id) const;
    const Subregion* findSubregion(int id) const;
    const Area* findArea(int id) const;
    const Province* findProvince(int id) const;
    bool areAdjacent(int provinceA, int provinceB) const;
    bool hasPassableConnection(int provinceA, int provinceB) const;
    int countOwnedProvinces(const std::string& tag) const;
    double developmentOf(const std::string& tag) const;

private:
    bool loadUnchecked(const std::string& dataDirectory, std::string& error);

    std::vector<Continent> continents_;
    std::vector<Region> regions_;
    std::vector<Subregion> subregions_;
    std::vector<Area> areas_;
    std::vector<Province> provinces_;
    std::vector<ProvinceConnection> connections_;
    std::vector<Country> countries_;
};

} // namespace eulite
