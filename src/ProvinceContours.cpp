#include "map/ProvinceContours.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

namespace eulite {
namespace {

struct VertexKey {
    std::uint32_t provinceId;
    std::uint32_t x;
    std::uint32_t y;

    bool operator==(const VertexKey& other) const {
        return provinceId == other.provinceId && x == other.x && y == other.y;
    }
};

struct VertexKeyHash {
    std::size_t operator()(const VertexKey& key) const {
        std::size_t value = static_cast<std::size_t>(key.provinceId);
        value ^= static_cast<std::size_t>(key.x) + 0x9e3779b9U +
                 (value << 6U) + (value >> 2U);
        value ^= static_cast<std::size_t>(key.y) + 0x9e3779b9U +
                 (value << 6U) + (value >> 2U);
        return value;
    }
};

struct BoundaryEdge {
    std::uint32_t provinceId = 0;
    std::uint32_t startX = 0;
    std::uint32_t startY = 0;
    std::uint32_t endX = 0;
    std::uint32_t endY = 0;
    unsigned char direction = 0; // east, south, west, north
    bool used = false;
};

void addEdge(std::vector<BoundaryEdge>& edges,
             std::unordered_map<VertexKey, std::vector<std::size_t>, VertexKeyHash>& outgoing,
             std::uint32_t provinceId,
             std::uint32_t startX, std::uint32_t startY,
             std::uint32_t endX, std::uint32_t endY,
             unsigned char direction) {
    const std::size_t index = edges.size();
    edges.push_back({provinceId, startX, startY, endX, endY, direction, false});
    outgoing[{provinceId, startX, startY}].push_back(index);
}

std::vector<ProvinceContourPoint> simplifyCollinear(
    const std::vector<ProvinceContourPoint>& points) {
    if (points.size() <= 4) return points;

    std::vector<ProvinceContourPoint> simplified;
    simplified.reserve(points.size());
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto& previous = points[(i + points.size() - 1U) % points.size()];
        const auto& current = points[i];
        const auto& next = points[(i + 1U) % points.size()];
        const bool sameX = previous.x == current.x && current.x == next.x;
        const bool sameY = previous.y == current.y && current.y == next.y;
        if (!sameX && !sameY) simplified.push_back(current);
    }
    return simplified.size() >= 4 ? simplified : points;
}

double calculateSignedArea(const std::vector<ProvinceContourPoint>& points) {
    long double twiceArea = 0.0L;
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto& a = points[i];
        const auto& b = points[(i + 1U) % points.size()];
        twiceArea += static_cast<long double>(a.x) * b.y -
                     static_cast<long double>(b.x) * a.y;
    }
    return static_cast<double>(twiceArea * 0.5L);
}

unsigned char chooseTurn(unsigned char incoming, unsigned char outgoing) {
    return static_cast<unsigned char>((outgoing + 4U - incoming) % 4U);
}

} // namespace

bool extractProvinceContours(const ProvinceRasterData& raster,
                             ProvinceContourData& output,
                             std::string& error,
                             bool ignoreProvinceIdZero) {
    const std::uint64_t expectedPixels =
        static_cast<std::uint64_t>(raster.width) * raster.height;
    if (raster.width == 0 || raster.height == 0 ||
        expectedPixels != raster.pixelProvinceIds.size()) {
        error = "Raster inválido: dimensões não correspondem à matriz de IDs.";
        return false;
    }

    std::vector<BoundaryEdge> edges;
    std::unordered_map<VertexKey, std::vector<std::size_t>, VertexKeyHash> outgoing;

    const auto pixelAt = [&raster](std::uint32_t x, std::uint32_t y) {
        return raster.pixelProvinceIds[static_cast<std::size_t>(y) * raster.width + x];
    };

    for (std::uint32_t y = 0; y < raster.height; ++y) {
        for (std::uint32_t x = 0; x < raster.width; ++x) {
            const std::uint32_t id = pixelAt(x, y);
            if (ignoreProvinceIdZero && id == 0) continue;

            const bool topDiffers = y == 0 || pixelAt(x, y - 1U) != id;
            const bool rightDiffers = x + 1U == raster.width || pixelAt(x + 1U, y) != id;
            const bool bottomDiffers = y + 1U == raster.height || pixelAt(x, y + 1U) != id;
            const bool leftDiffers = x == 0 || pixelAt(x - 1U, y) != id;

            // Directed edges keep the province interior on their right side.
            if (topDiffers)
                addEdge(edges, outgoing, id, x, y, x + 1U, y, 0);
            if (rightDiffers)
                addEdge(edges, outgoing, id, x + 1U, y, x + 1U, y + 1U, 1);
            if (bottomDiffers)
                addEdge(edges, outgoing, id, x + 1U, y + 1U, x, y + 1U, 2);
            if (leftDiffers)
                addEdge(edges, outgoing, id, x, y + 1U, x, y, 3);
        }
    }

    std::map<std::uint32_t, std::vector<ProvinceContourRing>> ringsByProvince;
    ProvinceContourData candidate;

    for (std::size_t startIndex = 0; startIndex < edges.size(); ++startIndex) {
        if (edges[startIndex].used) continue;

        const std::uint32_t provinceId = edges[startIndex].provinceId;
        const std::uint32_t startX = edges[startIndex].startX;
        const std::uint32_t startY = edges[startIndex].startY;
        std::size_t currentIndex = startIndex;
        std::vector<ProvinceContourPoint> points;
        points.reserve(32);
        bool closed = false;

        // Every valid boundary edge is consumed exactly once. The guard also
        // protects against malformed edge graphs rather than looping forever.
        for (std::size_t step = 0; step <= edges.size(); ++step) {
            BoundaryEdge& current = edges[currentIndex];
            if (current.used || current.provinceId != provinceId) break;
            current.used = true;
            points.push_back({current.startX, current.startY});

            if (current.endX == startX && current.endY == startY) {
                closed = true;
                break;
            }

            const auto found = outgoing.find({provinceId, current.endX, current.endY});
            if (found == outgoing.end()) break;

            std::size_t nextIndex = edges.size();
            unsigned int bestTurnRank = 5;
            for (const std::size_t candidateIndex : found->second) {
                const BoundaryEdge& next = edges[candidateIndex];
                if (next.used || next.provinceId != provinceId) continue;

                const unsigned char turn = chooseTurn(current.direction, next.direction);
                // Prefer right, straight, left, then reverse at diagonal-touch vertices.
                const unsigned int rank = turn == 1 ? 0U :
                                          turn == 0 ? 1U :
                                          turn == 3 ? 2U : 3U;
                if (rank < bestTurnRank) {
                    bestTurnRank = rank;
                    nextIndex = candidateIndex;
                }
            }
            if (nextIndex == edges.size()) break;
            currentIndex = nextIndex;
        }

        if (!closed || points.size() < 4) {
            error = "Não foi possível fechar um contorno da província " +
                    std::to_string(provinceId) + ".";
            return false;
        }

        points = simplifyCollinear(points);
        const double area = calculateSignedArea(points);
        if (points.size() < 4 || std::abs(area) < 0.5) {
            error = "Contorno degenerado encontrado na província " +
                    std::to_string(provinceId) + ".";
            return false;
        }

        ProvinceContourRing ring;
        ring.points = std::move(points);
        ring.signedArea = area;
        ring.isHole = area < 0.0;
        candidate.totalVertexCount += ring.points.size();
        ++candidate.totalRingCount;
        ringsByProvince[provinceId].push_back(std::move(ring));
    }

    candidate.provinces.reserve(ringsByProvince.size());
    for (auto& entry : ringsByProvince) {
        ProvincePolygon polygon;
        polygon.provinceId = entry.first;
        polygon.rings = std::move(entry.second);
        candidate.provinces.push_back(std::move(polygon));
    }

    output = std::move(candidate);
    error.clear();
    return true;
}


bool writeProvinceContoursSvg(const ProvinceContourData& contours,
                              std::uint32_t width,
                              std::uint32_t height,
                              const std::string& outputPath,
                              std::string& error) {
    if (width == 0 || height == 0) {
        error = "Dimensões inválidas para exportação SVG.";
        return false;
    }

    static const char* palette[] = {
        "#c89b55", "#5d86ad", "#b85b55", "#5c9678",
        "#9b7bb0", "#c1844d", "#5a999c", "#9a9d61"
    };

    std::ofstream output(outputPath);
    if (!output) {
        error = "Não foi possível criar o SVG: " + outputPath;
        return false;
    }

    output << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 "
           << width << ' ' << height << "\" role=\"img\" "
           << "aria-label=\"Mapa de províncias extraído do raster\">\n";
    output << "<rect width=\"100%\" height=\"100%\" fill=\"#b9ced1\"/>\n";

    for (const auto& province : contours.provinces) {
        if (province.rings.empty()) {
            error = "Polígono inválido na exportação SVG.";
            return false;
        }

        const char* color = palette[province.provinceId %
            (sizeof(palette) / sizeof(palette[0]))];
        output << "<path fill=\"" << color
               << "\" stroke=\"#26363b\" stroke-width=\"0.7\" "
               << "stroke-linejoin=\"round\" fill-rule=\"evenodd\" "
               << "data-province-id=\"" << province.provinceId << "\" d=\"";

        for (const auto& ring : province.rings) {
            if (ring.points.size() < 4) {
                error = "Anel com menos de quatro pontos na exportação SVG.";
                return false;
            }
            for (const auto& point : ring.points) {
                if (point.x > width || point.y > height) {
                    error = "Ponto fora das dimensões do raster na exportação SVG.";
                    return false;
                }
            }

            output << 'M' << ring.points.front().x << ' ' << ring.points.front().y;
            for (std::size_t i = 1; i < ring.points.size(); ++i) {
                output << 'L' << ring.points[i].x << ' ' << ring.points[i].y;
            }
            output << 'Z';
        }
        output << "\"/>\n";
    }

    output << "</svg>\n";
    if (!output) {
        error = "Falha ao gravar o SVG de províncias.";
        return false;
    }

    error.clear();
    return true;
}

} // namespace eulite
