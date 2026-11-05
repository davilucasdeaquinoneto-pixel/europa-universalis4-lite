#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace eulite {

struct ProvinceRasterBoundary {
    std::uint32_t provinceA = 0;
    std::uint32_t provinceB = 0;
};

struct ProvinceRasterData {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    // Pixels are stored top-to-bottom, left-to-right, as province IDs.
    std::vector<std::uint32_t> pixelProvinceIds;
    std::unordered_map<std::uint32_t, std::uint64_t> provincePixelCounts;
    std::vector<ProvinceRasterBoundary> boundaries;
};

// Imports an uncompressed 24-bit or 32-bit BMP province raster and a
// semicolon-delimited Paradox-style definition file (id;r;g;b;name;...).
// Exact RGB matching is used: antialiased or undefined colors are rejected.
bool importProvinceRasterBmp(const std::string& bitmapPath,
                             const std::string& definitionPath,
                             ProvinceRasterData& output,
                             std::string& error,
                             bool ignoreProvinceIdZero = true);

} // namespace eulite
