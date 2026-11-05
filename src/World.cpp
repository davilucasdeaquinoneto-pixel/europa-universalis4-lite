#include "world/World.hpp"

#include <fstream>
#include <sstream>
#include <unordered_set>

namespace eulite {
namespace {

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

bool parseDouble(const std::string& value, double& result) {
    try {
        std::size_t used = 0;
        result = std::stod(value, &used);
        return used == value.size();
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

} // namespace

bool World::load(const std::string& dataDirectory, std::string& error) {
    provinces_.clear();
    countries_.clear();

    std::ifstream countryFile(dataDirectory + "/countries.csv");
    if (!countryFile) {
        error = "Não foi possível abrir countries.csv em " + dataDirectory;
        return false;
    }

    std::string line;
    std::getline(countryFile, line); // Header
    std::unordered_set<std::string> countryTags;

    while (std::getline(countryFile, line)) {
        if (line.empty()) continue;
        const auto fields = splitCsvLine(line);
        if (fields.size() != 5) {
            error = "Linha inválida em countries.csv: " + line;
            return false;
        }

        Country country;
        if (!parseDouble(fields[2], country.treasury) ||
            !parseInt(fields[3], country.manpower) ||
            !parseDouble(fields[4], country.taxModifier)) {
            error = "Número inválido em countries.csv: " + line;
            return false;
        }
        country.tag = fields[0];
        country.name = fields[1];
        if (country.tag.empty() || country.name.empty() ||
            !countryTags.insert(country.tag).second) {
            error = "Tag ou nome vazio, ou tag duplicada: " + line;
            return false;
        }
        countries_.push_back(country);
    }

    std::ifstream provinceFile(dataDirectory + "/provinces.csv");
    if (!provinceFile) {
        error = "Não foi possível abrir provinces.csv em " + dataDirectory;
        return false;
    }

    std::getline(provinceFile, line); // Header
    std::unordered_set<int> provinceIds;
    while (std::getline(provinceFile, line)) {
        if (line.empty()) continue;
        const auto fields = splitCsvLine(line);
        if (fields.size() != 5) {
            error = "Linha inválida em provinces.csv: " + line;
            return false;
        }

        Province province;
        if (!parseInt(fields[0], province.id) ||
            !parseDouble(fields[3], province.development)) {
            error = "Número inválido em provinces.csv: " + line;
            return false;
        }
        province.name = fields[1];
        province.ownerTag = fields[2];
        province.terrain = fields[4];

        if (province.id <= 0 || province.name.empty() ||
            province.development <= 0.0 ||
            !provinceIds.insert(province.id).second) {
            error = "ID, nome ou desenvolvimento inválido, ou ID duplicado: " + line;
            return false;
        }
        if (!province.ownerTag.empty() && countryTags.count(province.ownerTag) == 0) {
            error = "País proprietário inexistente na província: " + province.name;
            return false;
        }
        provinces_.push_back(province);
    }

    if (countries_.empty()) {
        error = "countries.csv não contém países.";
        return false;
    }
    if (provinces_.empty()) {
        error = "provinces.csv não contém províncias.";
        return false;
    }
    return true;
}

Country* World::findCountry(const std::string& tag) {
    for (auto& country : countries_) {
        if (country.tag == tag) return &country;
    }
    return nullptr;
}

const Country* World::findCountry(const std::string& tag) const {
    for (const auto& country : countries_) {
        if (country.tag == tag) return &country;
    }
    return nullptr;
}

int World::countOwnedProvinces(const std::string& tag) const {
    int count = 0;
    for (const auto& province : provinces_) {
        if (province.ownerTag == tag) ++count;
    }
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
