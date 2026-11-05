#include "world/World.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>

namespace eulite {
namespace {

// CSV reader supporting quoted fields, commas inside quotes and escaped quotes ("").
// Quotes are only valid at the beginning of a field; characters after a closing
// quote must be a comma or the end of the line.
bool parseCsvLine(const std::string& line, std::vector<std::string>& fields) {
    fields.clear();
    std::string field;
    bool quoted = false;
    bool quoteClosed = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];

        if (quoted) {
            if (ch == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field.push_back('"');
                    ++i;
                } else {
                    quoted = false;
                    quoteClosed = true;
                }
            } else {
                field.push_back(ch);
            }
            continue;
        }

        if (quoteClosed) {
            if (ch != ',') return false;
            fields.push_back(field);
            field.clear();
            quoteClosed = false;
            continue;
        }

        if (ch == ',') {
            fields.push_back(field);
            field.clear();
        } else if (ch == '"') {
            if (!field.empty()) return false;
            quoted = true;
        } else {
            field.push_back(ch);
        }
    }

    if (quoted) return false;
    fields.push_back(field);
    return true;
}

bool parseDouble(const std::string& value, double& result) {
    try {
        std::size_t used = 0;
        result = std::stod(value, &used);
        return used == value.size() && std::isfinite(result);
    } catch (...) {
        return false;
    }
}

bool parseInt(const std::string& value, int& result) {
    try {
        std::size_t used = 0;
        result = std::stoi(value, &used);
        return used == value.size();
    } catch (...) {
        return false;
    }
}

bool readCsvFields(const std::string& line, std::vector<std::string>& fields,
                   bool stripBom = false) {
    std::string normalized = line;
    if (!normalized.empty() && normalized.back() == '\r') normalized.pop_back();
    if (stripBom && normalized.size() >= 3 &&
        static_cast<unsigned char>(normalized[0]) == 0xEF &&
        static_cast<unsigned char>(normalized[1]) == 0xBB &&
        static_cast<unsigned char>(normalized[2]) == 0xBF) {
        normalized.erase(0, 3);
    }
    return parseCsvLine(normalized, fields);
}

} // namespace

bool World::load(const std::string& dataDirectory, std::string& error) {
    World candidate;
    if (!candidate.loadUnchecked(dataDirectory, error)) return false;

    regions_.swap(candidate.regions_);
    provinces_.swap(candidate.provinces_);
    countries_.swap(candidate.countries_);
    error.clear();
    return true;
}

bool World::loadUnchecked(const std::string& dataDirectory, std::string& error) {
    regions_.clear();
    provinces_.clear();
    countries_.clear();

    std::ifstream countryFile(dataDirectory + "/countries.csv");
    if (!countryFile) {
        error = "Não foi possível abrir countries.csv em " + dataDirectory;
        return false;
    }

    std::string line;
    std::vector<std::string> fields;
    if (!std::getline(countryFile, line) || !readCsvFields(line, fields, true) ||
        fields != std::vector<std::string>{"tag", "name", "treasury", "manpower", "tax_modifier"}) {
        error = "Cabeçalho inválido em countries.csv.";
        return false;
    }

    std::unordered_set<std::string> countryTags;
    std::size_t lineNumber = 1;
    while (std::getline(countryFile, line)) {
        ++lineNumber;
        if (line.empty() || line == "\r") continue;
        if (!readCsvFields(line, fields) || fields.size() != 5) {
            error = "CSV inválido em countries.csv na linha " + std::to_string(lineNumber);
            return false;
        }

        Country country;
        if (!parseDouble(fields[2], country.treasury) ||
            !parseInt(fields[3], country.manpower) ||
            !parseDouble(fields[4], country.taxModifier)) {
            error = "Número inválido em countries.csv na linha " + std::to_string(lineNumber);
            return false;
        }
        country.tag = fields[0];
        country.name = fields[1];
        if (country.tag.empty() || country.name.empty() ||
            country.treasury < 0.0 || country.manpower < 0 ||
            country.taxModifier < 0.0 ||
            !countryTags.insert(country.tag).second) {
            error = "País inválido ou tag duplicada em countries.csv na linha " +
                    std::to_string(lineNumber);
            return false;
        }
        countries_.push_back(country);
    }

    std::ifstream regionFile(dataDirectory + "/regions.csv");
    if (!regionFile) {
        error = "Não foi possível abrir regions.csv em " + dataDirectory;
        return false;
    }

    if (!std::getline(regionFile, line) || !readCsvFields(line, fields, true) ||
        fields != std::vector<std::string>{"id", "name"}) {
        error = "Cabeçalho inválido em regions.csv.";
        return false;
    }

    std::unordered_set<int> regionIds;
    lineNumber = 1;
    while (std::getline(regionFile, line)) {
        ++lineNumber;
        if (line.empty() || line == "\r") continue;
        if (!readCsvFields(line, fields) || fields.size() != 2) {
            error = "CSV inválido em regions.csv na linha " + std::to_string(lineNumber);
            return false;
        }
        Region region;
        if (!parseInt(fields[0], region.id)) {
            error = "ID inválido em regions.csv na linha " + std::to_string(lineNumber);
            return false;
        }
        region.name = fields[1];
        if (region.id <= 0 || region.name.empty() ||
            !regionIds.insert(region.id).second) {
            error = "Região inválida ou ID duplicado em regions.csv na linha " +
                    std::to_string(lineNumber);
            return false;
        }
        regions_.push_back(region);
    }

    std::ifstream provinceFile(dataDirectory + "/provinces.csv");
    if (!provinceFile) {
        error = "Não foi possível abrir provinces.csv em " + dataDirectory;
        return false;
    }

    if (!std::getline(provinceFile, line) || !readCsvFields(line, fields, true) ||
        fields != std::vector<std::string>{"id", "name", "owner", "region_id", "development", "terrain"}) {
        error = "Cabeçalho inválido em provinces.csv.";
        return false;
    }

    std::unordered_set<int> provinceIds;
    std::unordered_map<int, std::size_t> provinceIndexById;
    lineNumber = 1;
    while (std::getline(provinceFile, line)) {
        ++lineNumber;
        if (line.empty() || line == "\r") continue;
        if (!readCsvFields(line, fields) || fields.size() != 6) {
            error = "CSV inválido em provinces.csv na linha " + std::to_string(lineNumber);
            return false;
        }

        Province province;
        if (!parseInt(fields[0], province.id) ||
            !parseInt(fields[3], province.regionId) ||
            !parseDouble(fields[4], province.development)) {
            error = "Número inválido em provinces.csv na linha " + std::to_string(lineNumber);
            return false;
        }
        province.name = fields[1];
        province.ownerTag = fields[2];
        province.terrain = fields[5];

        if (province.id <= 0 || province.name.empty() ||
            province.development <= 0.0 || province.terrain.empty() ||
            !provinceIds.insert(province.id).second) {
            error = "Província inválida ou ID duplicado em provinces.csv na linha " +
                    std::to_string(lineNumber);
            return false;
        }
        if (!province.ownerTag.empty() && countryTags.count(province.ownerTag) == 0) {
            error = "País proprietário inexistente na província: " + province.name;
            return false;
        }
        if (regionIds.count(province.regionId) == 0) {
            error = "Região inexistente na província: " + province.name;
            return false;
        }
        provinceIndexById.emplace(province.id, provinces_.size());
        provinces_.push_back(province);
    }

    std::ifstream adjacencyFile(dataDirectory + "/adjacencies.csv");
    if (!adjacencyFile) {
        error = "Não foi possível abrir adjacencies.csv em " + dataDirectory;
        return false;
    }

    if (!std::getline(adjacencyFile, line) || !readCsvFields(line, fields, true) ||
        fields != std::vector<std::string>{"province_a", "province_b"}) {
        error = "Cabeçalho inválido em adjacencies.csv.";
        return false;
    }

    std::set<std::pair<int, int>> connections;
    lineNumber = 1;
    while (std::getline(adjacencyFile, line)) {
        ++lineNumber;
        if (line.empty() || line == "\r") continue;
        if (!readCsvFields(line, fields) || fields.size() != 2) {
            error = "CSV inválido em adjacencies.csv na linha " + std::to_string(lineNumber);
            return false;
        }
        int provinceA = 0;
        int provinceB = 0;
        if (!parseInt(fields[0], provinceA) || !parseInt(fields[1], provinceB)) {
            error = "Número inválido em adjacencies.csv na linha " + std::to_string(lineNumber);
            return false;
        }
        if (provinceA == provinceB || provinceIds.count(provinceA) == 0 ||
            provinceIds.count(provinceB) == 0) {
            error = "Adjacência aponta para província inválida na linha " +
                    std::to_string(lineNumber);
            return false;
        }

        const auto edge = std::minmax(provinceA, provinceB);
        if (!connections.insert(edge).second) {
            error = "Adjacência duplicada em adjacencies.csv na linha " +
                    std::to_string(lineNumber);
            return false;
        }

        const auto indexA = provinceIndexById.find(provinceA);
        const auto indexB = provinceIndexById.find(provinceB);
        if (indexA == provinceIndexById.end() || indexB == provinceIndexById.end()) {
            error = "Índice interno de províncias inconsistente na linha " +
                    std::to_string(lineNumber);
            return false;
        }
        provinces_[indexA->second].adjacentProvinceIds.push_back(provinceB);
        provinces_[indexB->second].adjacentProvinceIds.push_back(provinceA);
    }

    if (countries_.empty() || regions_.empty() || provinces_.empty()) {
        error = "O cenário precisa conter países, regiões e províncias.";
        return false;
    }
    error.clear();
    return true;
}

Country* World::findCountry(const std::string& tag) {
    for (auto& country : countries_) if (country.tag == tag) return &country;
    return nullptr;
}

const Country* World::findCountry(const std::string& tag) const {
    for (const auto& country : countries_) if (country.tag == tag) return &country;
    return nullptr;
}

const Region* World::findRegion(int id) const {
    for (const auto& region : regions_) if (region.id == id) return &region;
    return nullptr;
}

const Province* World::findProvince(int id) const {
    for (const auto& province : provinces_) if (province.id == id) return &province;
    return nullptr;
}

bool World::areAdjacent(int provinceA, int provinceB) const {
    const Province* province = findProvince(provinceA);
    if (!province) return false;
    for (const int neighborId : province->adjacentProvinceIds) {
        if (neighborId == provinceB) return true;
    }
    return false;
}

int World::countOwnedProvinces(const std::string& tag) const {
    int count = 0;
    for (const auto& province : provinces_) if (province.ownerTag == tag) ++count;
    return count;
}

double World::developmentOf(const std::string& tag) const {
    double total = 0.0;
    for (const auto& province : provinces_) {
        if (province.ownerTag == tag) total += province.development;
    }
    return total;
}

} // namespace eulite
