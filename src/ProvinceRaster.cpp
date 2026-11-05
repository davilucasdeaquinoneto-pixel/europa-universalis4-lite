#include "map/ProvinceRaster.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <limits>
#include <sstream>
#include <unordered_set>
#include <utility>

namespace eulite {
namespace {

std::uint16_t readU16(const std::array<unsigned char, 54>& header, std::size_t offset) {
    return static_cast<std::uint16_t>(header[offset]) |
           (static_cast<std::uint16_t>(header[offset + 1]) << 8);
}

std::uint32_t readU32(const std::array<unsigned char, 54>& header, std::size_t offset) {
    return static_cast<std::uint32_t>(header[offset]) |
           (static_cast<std::uint32_t>(header[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(header[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(header[offset + 3]) << 24);
}

std::int32_t readI32(const std::array<unsigned char, 54>& header, std::size_t offset) {
    return static_cast<std::int32_t>(readU32(header, offset));
}

bool parseUnsigned(const std::string& value, std::uint32_t& result) {
    if (value.empty()) return false;
    try {
        std::size_t used = 0;
        const unsigned long long parsed = std::stoull(value, &used);
        if (used != value.size() ||
            parsed > std::numeric_limits<std::uint32_t>::max()) return false;
        result = static_cast<std::uint32_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool parseSemicolonLine(const std::string& line, std::vector<std::string>& fields) {
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
            if (ch != ';') return false;
            fields.push_back(field);
            field.clear();
            quoteClosed = false;
            continue;
        }
        if (ch == ';') {
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

std::uint32_t colorKey(unsigned char red, unsigned char green, unsigned char blue) {
    return (static_cast<std::uint32_t>(red) << 16) |
           (static_cast<std::uint32_t>(green) << 8) |
           static_cast<std::uint32_t>(blue);
}

bool loadDefinitions(const std::string& path,
                     std::unordered_map<std::uint32_t, std::uint32_t>& colorToId,
                     std::string& error) {
    std::ifstream file(path);
    if (!file) {
        error = "Não foi possível abrir o arquivo de definições: " + path;
        return false;
    }

    std::string line;
    std::vector<std::string> fields;
    std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lineNumber == 1 && line.size() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line.erase(0, 3);
        }
        if (line.empty()) continue;
        if (!parseSemicolonLine(line, fields)) {
            error = "Linha inválida no arquivo de definições: " +
                    std::to_string(lineNumber);
            return false;
        }

        std::uint32_t id = 0;
        if (fields.empty() || !parseUnsigned(fields[0], id)) {
            if (lineNumber == 1) continue; // Optional header row.
            error = "ID de província inválido na definição, linha " +
                    std::to_string(lineNumber);
            return false;
        }
        if (fields.size() < 4) {
            error = "Definição precisa conter ID e canais RGB, linha " +
                    std::to_string(lineNumber);
            return false;
        }

        std::uint32_t red = 0, green = 0, blue = 0;
        if (!parseUnsigned(fields[1], red) || !parseUnsigned(fields[2], green) ||
            !parseUnsigned(fields[3], blue) || red > 255 || green > 255 || blue > 255) {
            error = "Canal RGB inválido na definição, linha " +
                    std::to_string(lineNumber);
            return false;
        }

        const auto key = colorKey(static_cast<unsigned char>(red),
                                  static_cast<unsigned char>(green),
                                  static_cast<unsigned char>(blue));
        if (colorToId.count(key) != 0) {
            error = "Cor RGB duplicada no arquivo de definições, linha " +
                    std::to_string(lineNumber);
            return false;
        }
        for (const auto& entry : colorToId) {
            if (entry.second == id) {
                error = "ID de província duplicado no arquivo de definições, linha " +
                        std::to_string(lineNumber);
                return false;
            }
        }
        colorToId.emplace(key, id);
    }

    if (colorToId.empty()) {
        error = "O arquivo de definições não contém províncias.";
        return false;
    }
    return true;
}

} // namespace

bool importProvinceRasterBmp(const std::string& bitmapPath,
                             const std::string& definitionPath,
                             ProvinceRasterData& output,
                             std::string& error,
                             bool ignoreProvinceIdZero) {
    std::unordered_map<std::uint32_t, std::uint32_t> colorToId;
    if (!loadDefinitions(definitionPath, colorToId, error)) return false;

    std::ifstream bitmap(bitmapPath, std::ios::binary);
    if (!bitmap) {
        error = "Não foi possível abrir o raster BMP: " + bitmapPath;
        return false;
    }

    std::array<unsigned char, 54> header{};
    if (!bitmap.read(reinterpret_cast<char*>(header.data()),
                     static_cast<std::streamsize>(header.size()))) {
        error = "Cabeçalho BMP incompleto.";
        return false;
    }
    if (header[0] != 'B' || header[1] != 'M') {
        error = "Arquivo de raster não possui assinatura BMP.";
        return false;
    }

    const std::uint32_t pixelOffset = readU32(header, 10);
    const std::uint32_t dibSize = readU32(header, 14);
    const std::int32_t signedWidth = readI32(header, 18);
    const std::int32_t signedHeight = readI32(header, 22);
    const std::uint16_t planes = readU16(header, 26);
    const std::uint16_t bitsPerPixel = readU16(header, 28);
    const std::uint32_t compression = readU32(header, 30);

    if (dibSize < 40 || signedWidth <= 0 || signedHeight == 0 ||
        signedHeight == std::numeric_limits<std::int32_t>::min() ||
        planes != 1 || (bitsPerPixel != 24 && bitsPerPixel != 32) ||
        compression != 0 || pixelOffset < 14U + dibSize) {
        error = "BMP incompatível: esperado BMP Windows sem compressão, 24 ou 32 bits.";
        return false;
    }

    const std::uint64_t width = static_cast<std::uint32_t>(signedWidth);
    const std::uint64_t height = static_cast<std::uint64_t>(
        signedHeight < 0 ? -static_cast<std::int64_t>(signedHeight) : signedHeight);
    const std::uint64_t bytesPerPixel = bitsPerPixel / 8;
    const std::uint64_t rowBytes = width * bytesPerPixel;
    const std::uint64_t rowStride = ((rowBytes + 3U) / 4U) * 4U;
    const std::uint64_t pixelCount = width * height;
    const std::uint64_t rasterBytes = rowStride * height;

    if (width > std::numeric_limits<std::uint32_t>::max() ||
        height > std::numeric_limits<std::uint32_t>::max() ||
        pixelCount > std::numeric_limits<std::size_t>::max() ||
        rasterBytes > std::numeric_limits<std::size_t>::max() ||
        static_cast<std::uint64_t>(pixelOffset) + rasterBytes >
            static_cast<std::uint64_t>(std::numeric_limits<std::streamoff>::max())) {
        error = "Dimensões do BMP excedem os limites suportados.";
        return false;
    }

    bitmap.seekg(0, std::ios::end);
    const std::streamoff fileSize = bitmap.tellg();
    if (fileSize < 0 || static_cast<std::uint64_t>(fileSize) <
        static_cast<std::uint64_t>(pixelOffset) + rasterBytes) {
        error = "Dados de pixels BMP incompletos.";
        return false;
    }
    bitmap.seekg(static_cast<std::streamoff>(pixelOffset), std::ios::beg);
    if (!bitmap) {
        error = "Não foi possível localizar os pixels do BMP.";
        return false;
    }

    ProvinceRasterData candidate;
    candidate.width = static_cast<std::uint32_t>(width);
    candidate.height = static_cast<std::uint32_t>(height);
    candidate.pixelProvinceIds.resize(static_cast<std::size_t>(pixelCount));
    std::vector<unsigned char> row(static_cast<std::size_t>(rowStride));

    for (std::uint64_t fileY = 0; fileY < height; ++fileY) {
        if (!bitmap.read(reinterpret_cast<char*>(row.data()),
                         static_cast<std::streamsize>(row.size()))) {
            error = "Falha ao ler uma linha de pixels do BMP.";
            return false;
        }
        const std::uint64_t y = signedHeight > 0 ? height - 1U - fileY : fileY;
        for (std::uint64_t x = 0; x < width; ++x) {
            const std::size_t offset = static_cast<std::size_t>(x * bytesPerPixel);
            const auto key = colorKey(row[offset + 2], row[offset + 1], row[offset]);
            const auto found = colorToId.find(key);
            if (found == colorToId.end()) {
                error = "Cor RGB sem definição no raster, na posição (" +
                        std::to_string(x) + ", " + std::to_string(y) + ").";
                return false;
            }
            const std::uint32_t id = found->second;
            candidate.pixelProvinceIds[static_cast<std::size_t>(y * width + x)] = id;
            if (!(ignoreProvinceIdZero && id == 0)) {
                ++candidate.provincePixelCounts[id];
            }
        }
    }

    std::unordered_set<std::uint64_t> edgeKeys;
    const auto addBoundary = [&edgeKeys, &candidate, ignoreProvinceIdZero](
        std::uint32_t a, std::uint32_t b) {
        if (a == b || (ignoreProvinceIdZero && (a == 0 || b == 0))) return;
        const std::uint32_t low = std::min(a, b);
        const std::uint32_t high = std::max(a, b);
        const std::uint64_t key = (static_cast<std::uint64_t>(low) << 32) | high;
        edgeKeys.insert(key);
    };

    for (std::uint32_t y = 0; y < candidate.height; ++y) {
        for (std::uint32_t x = 0; x < candidate.width; ++x) {
            const std::size_t index = static_cast<std::size_t>(y) * candidate.width + x;
            const std::uint32_t id = candidate.pixelProvinceIds[index];
            if (x + 1U < candidate.width) {
                addBoundary(id, candidate.pixelProvinceIds[index + 1U]);
            }
            if (y + 1U < candidate.height) {
                addBoundary(id, candidate.pixelProvinceIds[index + candidate.width]);
            }
        }
    }

    candidate.boundaries.reserve(edgeKeys.size());
    for (const auto key : edgeKeys) {
        candidate.boundaries.push_back({
            static_cast<std::uint32_t>(key >> 32),
            static_cast<std::uint32_t>(key & 0xffffffffU)
        });
    }
    std::sort(candidate.boundaries.begin(), candidate.boundaries.end(),
        [](const ProvinceRasterBoundary& a, const ProvinceRasterBoundary& b) {
            if (a.provinceA != b.provinceA) return a.provinceA < b.provinceA;
            return a.provinceB < b.provinceB;
        });

    output = std::move(candidate);
    error.clear();
    return true;
}

} // namespace eulite
