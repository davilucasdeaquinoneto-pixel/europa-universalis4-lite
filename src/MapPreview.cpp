#include "map/MapPreview.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace eulite {
namespace {

struct LayoutPoint {
    int provinceId = 0;
    double gridX = 0.0;
    double gridY = 0.0;
    double elevation = 0.0;
};

struct ProjectedProvince {
    const Province* province = nullptr;
    double x = 0.0;
    double y = 0.0;
    double depth = 0.0;
    double sortKey = 0.0;
};

bool parseLayoutRow(const std::string& line, LayoutPoint& point) {
    std::istringstream input(line);
    std::string idText, xText, yText, elevationText, extra;
    if (!std::getline(input, idText, ',') ||
        !std::getline(input, xText, ',') ||
        !std::getline(input, yText, ',') ||
        !std::getline(input, elevationText, ',') ||
        std::getline(input, extra, ',')) {
        return false;
    }

    try {
        std::size_t used = 0;
        point.provinceId = std::stoi(idText, &used);
        if (used != idText.size()) return false;

        used = 0;
        point.gridX = std::stod(xText, &used);
        if (used != xText.size() || !std::isfinite(point.gridX)) return false;

        used = 0;
        point.gridY = std::stod(yText, &used);
        if (used != yText.size() || !std::isfinite(point.gridY)) return false;

        used = 0;
        point.elevation = std::stod(elevationText, &used);
        if (used != elevationText.size() || !std::isfinite(point.elevation)) return false;
    } catch (...) {
        return false;
    }

    return point.provinceId > 0 && point.elevation >= 0.0;
}

std::string xmlEscape(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (const char ch : value) {
        switch (ch) {
        case '&': escaped += "&amp;"; break;
        case '<': escaped += "&lt;"; break;
        case '>': escaped += "&gt;"; break;
        case '"': escaped += "&quot;"; break;
        case '\'': escaped += "&apos;"; break;
        default: escaped += ch; break;
        }
    }
    return escaped;
}

std::string countryColor(const std::string& tag) {
    static const char* palette[] = {
        "#bd914d", "#527da6", "#a95050", "#4e896c",
        "#9270a8", "#b77b48", "#538d91", "#8b8e53"
    };
    // FNV-1a keeps political colors stable across runs and platforms.
    std::uint64_t hash = 1469598103934665603ULL;
    for (const unsigned char ch : tag) {
        hash ^= ch;
        hash *= 1099511628211ULL;
    }
    const std::size_t index = static_cast<std::size_t>(
        hash % (sizeof(palette) / sizeof(palette[0])));
    return palette[index];
}

std::string darken(const std::string& color) {
    if (color.size() != 7 || color[0] != '#') return "#39434a";
    unsigned int red = 0, green = 0, blue = 0;
    std::istringstream redInput(color.substr(1, 2));
    redInput >> std::hex >> red;
    std::istringstream greenInput(color.substr(3, 2));
    greenInput >> std::hex >> green;
    std::istringstream blueInput(color.substr(5, 2));
    blueInput >> std::hex >> blue;

    red = red * 62 / 100;
    green = green * 62 / 100;
    blue = blue * 62 / 100;

    std::ostringstream output;
    output << '#' << std::hex << std::setfill('0')
           << std::setw(2) << red << std::setw(2) << green << std::setw(2) << blue;
    return output.str();
}

} // namespace

bool writeMapPreviewSvg(const World& world,
                        const std::string& layoutPath,
                        const std::string& outputPath,
                        std::string& error) {
    std::ifstream layoutFile(layoutPath);
    if (!layoutFile) {
        error = "Não foi possível abrir o layout do mapa: " + layoutPath;
        return false;
    }

    std::string line;
    if (!std::getline(layoutFile, line)) {
        error = "Arquivo de layout vazio.";
        return false;
    }
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line != "province_id,grid_x,grid_y,elevation") {
        error = "Cabeçalho inválido em map_layout.csv.";
        return false;
    }

    std::unordered_map<int, LayoutPoint> layout;
    std::size_t lineNumber = 1;
    while (std::getline(layoutFile, line)) {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        LayoutPoint point;
        if (!parseLayoutRow(line, point) ||
            !layout.emplace(point.provinceId, point).second) {
            error = "Linha inválida ou ID duplicado em map_layout.csv: " +
                    std::to_string(lineNumber);
            return false;
        }
    }

    std::vector<ProjectedProvince> projected;
    projected.reserve(world.provinces().size());
    std::unordered_set<std::string> ownerTags;
    double minX = 0.0, minY = 0.0, maxX = 0.0, maxY = 0.0;
    bool first = true;

    constexpr double halfWidth = 35.0;
    constexpr double halfHeight = 17.5;
    for (const auto& province : world.provinces()) {
        const auto found = layout.find(province.id);
        if (found == layout.end()) {
            error = "Província sem posição no map_layout.csv: " +
                    std::to_string(province.id);
            return false;
        }

        const auto& point = found->second;
        const double x = (point.gridX - point.gridY) * halfWidth * 2.0;
        const double y = (point.gridX + point.gridY) * halfHeight * 2.0 -
                         point.elevation * 7.0;
        const double depth = 12.0 + point.elevation * 2.0;

        projected.push_back({&province, x, y, depth, point.gridX + point.gridY});
        if (!province.ownerTag.empty()) ownerTags.insert(province.ownerTag);

        const double left = x - halfWidth;
        const double right = x + halfWidth;
        const double bottom = y + halfHeight * 2.0 + depth;
        if (first) {
            minX = left; maxX = right; minY = y; maxY = bottom;
            first = false;
        } else {
            minX = std::min(minX, left);
            maxX = std::max(maxX, right);
            minY = std::min(minY, y);
            maxY = std::max(maxY, bottom);
        }
    }

    if (projected.empty()) {
        error = "Não há províncias para desenhar.";
        return false;
    }

    std::stable_sort(projected.begin(), projected.end(),
        [](const ProjectedProvince& a, const ProjectedProvince& b) {
            return a.sortKey < b.sortKey;
        });

    constexpr double margin = 72.0;
    const double width = maxX - minX + margin * 2.0;
    const double height = maxY - minY + margin * 2.0 + 42.0;
    const double offsetX = margin - minX;
    const double offsetY = margin + 42.0 - minY;

    std::ofstream output(outputPath);
    if (!output) {
        error = "Não foi possível criar o mapa: " + outputPath;
        return false;
    }

    output << std::fixed << std::setprecision(1);
    output << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 "
           << width << ' ' << height << "\" role=\"img\" "
           << "aria-label=\"Mapa isométrico demonstrativo do Europa Universalis Lite\">\n";
    output << "<rect width=\"100%\" height=\"100%\" fill=\"#c7d7d8\"/>\n";
    output << "<path d=\"M0 " << height * 0.72 << " Q" << width * 0.5 << ' '
           << height * 0.58 << ' ' << width << ' ' << height * 0.72
           << "\" fill=\"none\" stroke=\"#b5c9cb\" stroke-width=\"2\"/>\n";
    output << "<text x=\"28\" y=\"34\" font-family=\"Georgia,serif\" "
           << "font-size=\"23\" fill=\"#26343a\">EUROPA UNIVERSALIS LITE</text>\n";
    output << "<text x=\"29\" y=\"53\" font-family=\"Arial,sans-serif\" "
           << "font-size=\"10\" letter-spacing=\"1.5\" fill=\"#52666b\">"
           << "MAPA ISOMÉTRICO • PROTÓTIPO DE APRESENTAÇÃO</text>\n";

    for (const auto& item : projected) {
        const auto& province = *item.province;
        const double x = item.x + offsetX;
        const double y = item.y + offsetY;
        const double d = item.depth;
        const std::string top = countryColor(province.ownerTag);
        const std::string side = darken(top);

        // Two visible vertical faces create the low-poly, EU3-era block-map effect.
        output << "<polygon points=\""
               << x - halfWidth << ',' << y + halfHeight
               << ' ' << x << ',' << y + halfHeight * 2.0
               << ' ' << x << ',' << y + halfHeight * 2.0 + d
               << ' ' << x - halfWidth << ',' << y + halfHeight + d
               << "\" fill=\"" << side << "\" stroke=\"#34434a\" stroke-width=\"1\"/>\n";
        output << "<polygon points=\""
               << x << ',' << y + halfHeight * 2.0
               << ' ' << x + halfWidth << ',' << y + halfHeight
               << ' ' << x + halfWidth << ',' << y + halfHeight + d
               << ' ' << x << ',' << y + halfHeight * 2.0 + d
               << "\" fill=\"" << darken(side) << "\" stroke=\"#34434a\" stroke-width=\"1\"/>\n";
        output << "<polygon points=\""
               << x << ',' << y
               << ' ' << x + halfWidth << ',' << y + halfHeight
               << ' ' << x << ',' << y + halfHeight * 2.0
               << ' ' << x - halfWidth << ',' << y + halfHeight
               << "\" fill=\"" << top
               << "\" stroke=\"#26363b\" stroke-width=\"1.5\"/>\n";

        output << "<text x=\"" << x << "\" y=\"" << y + 17.0
               << "\" text-anchor=\"middle\" font-family=\"Arial,sans-serif\" "
               << "font-size=\"10\" font-weight=\"bold\" fill=\"#18262b\">"
               << xmlEscape(province.name) << "</text>\n";
        output << "<text x=\"" << x << "\" y=\"" << y + 28.0
               << "\" text-anchor=\"middle\" font-family=\"Arial,sans-serif\" "
               << "font-size=\"7\" fill=\"#18262b\">"
               << xmlEscape(province.ownerTag) << " • " << province.id << "</text>\n";
    }

    std::vector<std::string> sortedOwnerTags(ownerTags.begin(), ownerTags.end());
    std::sort(sortedOwnerTags.begin(), sortedOwnerTags.end());
    double legendX = 30.0;
    const double legendY = height - 25.0;
    for (const auto& tag : sortedOwnerTags) {
        const Country* country = world.findCountry(tag);
        output << "<rect x=\"" << legendX << "\" y=\"" << legendY - 9.0
               << "\" width=\"10\" height=\"10\" rx=\"2\" fill=\""
               << countryColor(tag) << "\"/>\n";
        output << "<text x=\"" << legendX + 15.0 << "\" y=\"" << legendY
               << "\" font-family=\"Arial,sans-serif\" font-size=\"10\" "
               << "fill=\"#26343a\">" << xmlEscape(country ? country->name : tag)
               << "</text>\n";
        legendX += country ? (country->name.size() * 6.2 + 34.0) : 65.0;
    }

    output << "<text x=\"" << width - 18.0 << "\" y=\"" << height - 8.0
           << "\" text-anchor=\"end\" font-family=\"Arial,sans-serif\" "
           << "font-size=\"8\" fill=\"#52666b\">Layout fictício de demonstração • "
           << "não representa fronteiras históricas</text>\n";
    output << "</svg>\n";

    if (!output) {
        error = "Falha ao gravar o arquivo SVG.";
        return false;
    }

    error.clear();
    return true;
}

} // namespace eulite
