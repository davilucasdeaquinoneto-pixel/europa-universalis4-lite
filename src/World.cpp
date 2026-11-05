#include "world/World.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
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

bool parseConnectionType(const std::string& value, ConnectionType& type) {
    if (value == "land") type = ConnectionType::Land;
    else if (value == "sea") type = ConnectionType::Sea;
    else if (value == "strait") type = ConnectionType::Strait;
    else if (value == "river") type = ConnectionType::River;
    else if (value == "impassable") type = ConnectionType::Impassable;
    else return false;
    return true;
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

    continents_.swap(candidate.continents_);
    regions_.swap(candidate.regions_);
    subregions_.swap(candidate.subregions_);
    areas_.swap(candidate.areas_);
    provinces_.swap(candidate.provinces_);
    connections_.swap(candidate.connections_);
    countries_.swap(candidate.countries_);
    error.clear();
    return true;
}

bool World::loadUnchecked(const std::string& dataDirectory, std::string& error) {
    continents_.clear();
    regions_.clear();
    subregions_.clear();
    areas_.clear();
    provinces_.clear();
    connections_.clear();
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

    // Optional hierarchy metadata. The legacy regions.csv remains the source
    // for region names and IDs; geography.csv adds parent relationships.
    std::ifstream geographyFile(dataDirectory + "/geography.csv");
    if (geographyFile) {
        if (!std::getline(geographyFile, line) || !readCsvFields(line, fields, true) ||
            fields != std::vector<std::string>{"type", "id", "name", "parent_id"}) {
            error = "Cabeçalho inválido em geography.csv.";
            return false;
        }

        std::unordered_set<int> continentIds;
        std::unordered_set<int> subregionIds;
        std::unordered_set<int> areaIds;
        lineNumber = 1;
        while (std::getline(geographyFile, line)) {
            ++lineNumber;
            if (line.empty() || line == "\r") continue;
            if (!readCsvFields(line, fields) || fields.size() != 4) {
                error = "CSV inválido em geography.csv na linha " + std::to_string(lineNumber);
                return false;
            }

            int id = 0;
            int parentId = 0;
            if (!parseInt(fields[1], id) || !parseInt(fields[3], parentId) ||
                id <= 0 || fields[2].empty()) {
                error = "ID, nome ou pai inválido em geography.csv na linha " +
                        std::to_string(lineNumber);
                return false;
            }

            if (fields[0] == "continent") {
                if (parentId != 0 || !continentIds.insert(id).second) {
                    error = "Continente inválido ou duplicado em geography.csv na linha " +
                            std::to_string(lineNumber);
                    return false;
                }
                continents_.push_back({id, fields[2]});
            } else if (fields[0] == "region") {
                auto region = std::find_if(regions_.begin(), regions_.end(),
                    [id](const Region& value) { return value.id == id; });
                if (region == regions_.end() || region->name != fields[2] ||
                    region->continentId != 0) {
                    error = "Região inexistente, duplicada ou com nome divergente em geography.csv na linha " +
                            std::to_string(lineNumber);
                    return false;
                }
                region->continentId = parentId;
            } else if (fields[0] == "subregion") {
                if (!subregionIds.insert(id).second) {
                    error = "Sub-região duplicada em geography.csv na linha " +
                            std::to_string(lineNumber);
                    return false;
                }
                subregions_.push_back({id, fields[2], parentId});
            } else if (fields[0] == "area") {
                if (!areaIds.insert(id).second) {
                    error = "Área duplicada em geography.csv na linha " +
                            std::to_string(lineNumber);
                    return false;
                }
                areas_.push_back({id, fields[2], parentId});
            } else {
                error = "Tipo geográfico desconhecido em geography.csv na linha " +
                        std::to_string(lineNumber);
                return false;
            }
        }

        if (continents_.empty() || subregions_.empty() || areas_.empty()) {
            error = "geography.csv precisa definir continentes, sub-regiões e áreas.";
            return false;
        }
        for (const auto& region : regions_) {
            if (region.continentId == 0 || continentIds.count(region.continentId) == 0) {
                error = "Região sem continente válido: " + region.name;
                return false;
            }
        }
        for (const auto& subregion : subregions_) {
            if (std::none_of(regions_.begin(), regions_.end(),
                [&subregion](const Region& region) { return region.id == subregion.regionId; })) {
                error = "Sub-região aponta para região inexistente: " + subregion.name;
                return false;
            }
        }
        for (const auto& area : areas_) {
            if (subregionIds.count(area.subregionId) == 0) {
                error = "Área aponta para sub-região inexistente: " + area.name;
                return false;
            }
        }
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

    if (!continents_.empty()) {
        std::ifstream provinceAreasFile(dataDirectory + "/province_areas.csv");
        if (!provinceAreasFile ||
            !std::getline(provinceAreasFile, line) ||
            !readCsvFields(line, fields, true) ||
            fields != std::vector<std::string>{"province_id", "area_id"}) {
            error = "geography.csv exige province_areas.csv com cabeçalho válido.";
            return false;
        }

        std::unordered_set<int> mappedProvinceIds;
        lineNumber = 1;
        while (std::getline(provinceAreasFile, line)) {
            ++lineNumber;
            if (line.empty() || line == "\r") continue;
            if (!readCsvFields(line, fields) || fields.size() != 2) {
                error = "CSV inválido em province_areas.csv na linha " +
                        std::to_string(lineNumber);
                return false;
            }
            int provinceId = 0;
            int areaId = 0;
            if (!parseInt(fields[0], provinceId) || !parseInt(fields[1], areaId) ||
                !mappedProvinceIds.insert(provinceId).second) {
                error = "Mapeamento de área inválido ou duplicado na linha " +
                        std::to_string(lineNumber);
                return false;
            }
            auto province = std::find_if(provinces_.begin(), provinces_.end(),
                [provinceId](const Province& value) { return value.id == provinceId; });
            auto area = std::find_if(areas_.begin(), areas_.end(),
                [areaId](const Area& value) { return value.id == areaId; });
            if (province == provinces_.end() || area == areas_.end()) {
                error = "Mapeamento aponta para província ou área inexistente na linha " +
                        std::to_string(lineNumber);
                return false;
            }
            auto subregion = std::find_if(subregions_.begin(), subregions_.end(),
                [area](const Subregion& value) { return value.id == area->subregionId; });
            if (subregion == subregions_.end() || subregion->regionId != province->regionId) {
                error = "Área e província pertencem a regiões incompatíveis: " + province->name;
                return false;
            }
            province->areaId = areaId;
        }

        if (mappedProvinceIds.size() != provinces_.size()) {
            error = "Toda província precisa estar associada a uma área em province_areas.csv.";
            return false;
        }
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

    std::unordered_set<std::uint64_t> connections;
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

        const int lowerId = std::min(provinceA, provinceB);
        const int upperId = std::max(provinceA, provinceB);
        const std::uint64_t edgeKey =
            (static_cast<std::uint64_t>(static_cast<std::uint32_t>(lowerId)) << 32) |
            static_cast<std::uint32_t>(upperId);
        if (!connections.insert(edgeKey).second) {
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
        connections_.push_back({provinceA, provinceB, ConnectionType::Land, false});
    }

    // Optional typed links extend the legacy land-adjacency file.
    std::ifstream typedConnectionsFile(dataDirectory + "/connections.csv");
    if (typedConnectionsFile) {
        if (!std::getline(typedConnectionsFile, line) ||
            !readCsvFields(line, fields, true) ||
            fields != std::vector<std::string>{"province_a", "province_b", "type", "one_way"}) {
            error = "Cabeçalho inválido em connections.csv.";
            return false;
        }

        std::unordered_set<std::string> typedKeys;
        for (const auto& connection : connections_) {
            const int low = std::min(connection.provinceA, connection.provinceB);
            const int high = std::max(connection.provinceA, connection.provinceB);
            typedKeys.insert(std::to_string(low) + ":" + std::to_string(high) + ":land");
        }

        lineNumber = 1;
        while (std::getline(typedConnectionsFile, line)) {
            ++lineNumber;
            if (line.empty() || line == "\r") continue;
            if (!readCsvFields(line, fields) || fields.size() != 4) {
                error = "CSV inválido em connections.csv na linha " +
                        std::to_string(lineNumber);
                return false;
            }

            int provinceA = 0;
            int provinceB = 0;
            int oneWayValue = 0;
            ConnectionType type;
            if (!parseInt(fields[0], provinceA) || !parseInt(fields[1], provinceB) ||
                !parseConnectionType(fields[2], type) ||
                !parseInt(fields[3], oneWayValue) ||
                (oneWayValue != 0 && oneWayValue != 1) ||
                provinceA == provinceB || provinceIds.count(provinceA) == 0 ||
                provinceIds.count(provinceB) == 0) {
                error = "Conexão inválida em connections.csv na linha " +
                        std::to_string(lineNumber);
                return false;
            }

            const int low = std::min(provinceA, provinceB);
            const int high = std::max(provinceA, provinceB);
            const std::string key = std::to_string(low) + ":" +
                                    std::to_string(high) + ":" + fields[2];
            if (!typedKeys.insert(key).second) {
                error = "Conexão duplicada em connections.csv na linha " +
                        std::to_string(lineNumber);
                return false;
            }

            const bool oneWay = oneWayValue == 1;
            connections_.push_back({provinceA, provinceB, type, oneWay});
            const auto indexA = provinceIndexById.at(provinceA);
            const auto indexB = provinceIndexById.at(provinceB);
            auto& neighborsA = provinces_[indexA].adjacentProvinceIds;
            if (std::find(neighborsA.begin(), neighborsA.end(), provinceB) == neighborsA.end()) {
                neighborsA.push_back(provinceB);
            }
            if (!oneWay) {
                auto& neighborsB = provinces_[indexB].adjacentProvinceIds;
                if (std::find(neighborsB.begin(), neighborsB.end(), provinceA) == neighborsB.end()) {
                    neighborsB.push_back(provinceA);
                }
            }
        }
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

const Continent* World::findContinent(int id) const {
    for (const auto& continent : continents_) if (continent.id == id) return &continent;
    return nullptr;
}

const Region* World::findRegion(int id) const {
    for (const auto& region : regions_) if (region.id == id) return &region;
    return nullptr;
}

const Subregion* World::findSubregion(int id) const {
    for (const auto& subregion : subregions_) if (subregion.id == id) return &subregion;
    return nullptr;
}

const Area* World::findArea(int id) const {
    for (const auto& area : areas_) if (area.id == id) return &area;
    return nullptr;
}

const Province* World::findProvince(int id) const {
    for (const auto& province : provinces_) if (province.id == id) return &province;
    return nullptr;
}

bool World::areAdjacent(int provinceA, int provinceB) const {
    for (const auto& connection : connections_) {
        if ((connection.provinceA == provinceA && connection.provinceB == provinceB) ||
            (connection.provinceA == provinceB && connection.provinceB == provinceA)) {
            return true;
        }
    }
    return false;
}

bool World::hasPassableConnection(int provinceA, int provinceB) const {
    for (const auto& connection : connections_) {
        if (connection.type == ConnectionType::Impassable) continue;
        if (connection.provinceA == provinceA && connection.provinceB == provinceB) return true;
        if (!connection.oneWay && connection.provinceA == provinceB &&
            connection.provinceB == provinceA) return true;
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
