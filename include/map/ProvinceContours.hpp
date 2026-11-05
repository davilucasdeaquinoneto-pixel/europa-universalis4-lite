#pragma once

#include "map/ProvinceRaster.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace eulite {

struct ProvinceContourPoint {
    std::uint32_t x = 0;
    std::uint32_t y = 0;
};

struct ProvinceContourRing {
    // Vertices are in raster coordinates. The closing point is not repeated.
    // Outer rings are clockwise in screen coordinates; holes are counter-clockwise.
    std::vector<ProvinceContourPoint> points;
    double signedArea = 0.0;
    bool isHole = false;
};

struct ProvincePolygon {
    std::uint32_t provinceId = 0;
    // A province can have multiple disconnected islands and holes.
    std::vector<ProvinceContourRing> rings;
};

struct ProvinceContourData {
    std::vector<ProvincePolygon> provinces;
    std::size_t totalRingCount = 0;
    std::size_t totalVertexCount = 0;
};

// Extracts exact pixel-edge contours. Collinear vertices are removed without
// changing the raster boundary. The input and output are left unchanged on failure.
bool extractProvinceContours(const ProvinceRasterData& raster,
                             ProvinceContourData& output,
                             std::string& error,
                             bool ignoreProvinceIdZero = true);

// Writes a flat SVG preview. evenodd fill keeps holes transparent.
bool writeProvinceContoursSvg(const ProvinceContourData& contours,
                              std::uint32_t width,
                              std::uint32_t height,
                              const std::string& outputPath,
                              std::string& error);

} // namespace eulite
