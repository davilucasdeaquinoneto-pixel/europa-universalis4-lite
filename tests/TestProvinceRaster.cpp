#include "map/ProvinceRaster.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

void put16(std::vector<unsigned char>& bytes, std::size_t offset, std::uint16_t value) {
    bytes[offset] = static_cast<unsigned char>(value & 0xffU);
    bytes[offset + 1] = static_cast<unsigned char>((value >> 8) & 0xffU);
}

void put32(std::vector<unsigned char>& bytes, std::size_t offset, std::uint32_t value) {
    for (unsigned int i = 0; i < 4; ++i) {
        bytes[offset + i] = static_cast<unsigned char>((value >> (i * 8)) & 0xffU);
    }
}

bool writeBmp(const std::filesystem::path& path, std::uint32_t width,
              std::int32_t signedHeight, std::uint16_t bitsPerPixel,
              const std::vector<Pixel>& topDownPixels) {
    const std::uint32_t height = static_cast<std::uint32_t>(
        signedHeight < 0 ? -static_cast<std::int64_t>(signedHeight) : signedHeight);
    const std::uint32_t bytesPerPixel = bitsPerPixel / 8U;
    const std::uint32_t rowBytes = width * bytesPerPixel;
    const std::uint32_t rowStride = (rowBytes + 3U) & ~3U;
    const std::uint32_t imageSize = rowStride * height;
    std::vector<unsigned char> bytes(54U + imageSize, 0);

    bytes[0] = 'B';
    bytes[1] = 'M';
    put32(bytes, 2, static_cast<std::uint32_t>(bytes.size()));
    put32(bytes, 10, 54);
    put32(bytes, 14, 40);
    put32(bytes, 18, width);
    put32(bytes, 22, static_cast<std::uint32_t>(signedHeight));
    put16(bytes, 26, 1);
    put16(bytes, 28, bitsPerPixel);
    put32(bytes, 30, 0);
    put32(bytes, 34, imageSize);

    for (std::uint32_t fileY = 0; fileY < height; ++fileY) {
        const std::uint32_t y = signedHeight > 0 ? height - 1U - fileY : fileY;
        for (std::uint32_t x = 0; x < width; ++x) {
            const Pixel& pixel = topDownPixels[static_cast<std::size_t>(y) * width + x];
            const std::size_t offset = 54U +
                static_cast<std::size_t>(fileY) * rowStride +
                static_cast<std::size_t>(x) * bytesPerPixel;
            bytes[offset] = pixel.blue;
            bytes[offset + 1] = pixel.green;
            bytes[offset + 2] = pixel.red;
            if (bitsPerPixel == 32) bytes[offset + 3] = 255;
        }
    }

    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(file);
}

bool writeDefinitions(const std::filesystem::path& path) {
    std::ofstream file(path);
    file << "province;red;green;blue;name;x;x;x\n"
         << "1;255;0;0;Madrid;0;0;0\n"
         << "2;0;255;0;Paris;0;0;0\n"
         << "3;0;0;255;Toledo;0;0;0\n"
         << "0;0;0;0;Ocean;0;0;0\n";
    return static_cast<bool>(file);
}

bool check(bool condition, const std::string& label) {
    if (!condition) std::cerr << "FALHOU: " << label << "\n";
    return condition;
}

} // namespace

int main() {
    const auto root = std::filesystem::temp_directory_path() /
                      "europa-lite-province-raster-tests";
    std::error_code ec;
    std::filesystem::remove_all(root, ec);
    std::filesystem::create_directories(root);

    const auto definitions = root / "definition.csv";
    const auto bitmap = root / "provinces.bmp";
    const auto invalidBitmap = root / "invalid.bmp";
    const auto topDownBitmap = root / "top-down-32.bmp";
    bool passed = writeDefinitions(definitions);

    const std::vector<Pixel> pixels = {
        {255, 0, 0}, {255, 0, 0}, {0, 255, 0},
        {0, 0, 255}, {0, 255, 0}, {0, 255, 0}
    };
    passed &= check(writeBmp(bitmap, 3, 2, 24, pixels),
                    "gerar BMP 24-bit com padding de linha");
    eulite::ProvinceRasterData raster;
    std::string error;
    passed &= check(eulite::importProvinceRasterBmp(bitmap.string(), definitions.string(),
                                                    raster, error),
                    "importar BMP 24-bit: " + error);
    passed &= check(raster.width == 3 && raster.height == 2,
                    "dimensões do raster");
    passed &= check(raster.pixelProvinceIds ==
                    std::vector<std::uint32_t>{1, 1, 2, 3, 2, 2},
                    "orientação top-down e IDs RGB");
    passed &= check(raster.provincePixelCounts[1] == 2 &&
                    raster.provincePixelCounts[2] == 3 &&
                    raster.provincePixelCounts[3] == 1,
                    "contagem de pixels por província");
    passed &= check(raster.boundaries.size() == 3 &&
                    raster.boundaries[0].provinceA == 1 &&
                    raster.boundaries[0].provinceB == 2 &&
                    raster.boundaries[1].provinceA == 1 &&
                    raster.boundaries[1].provinceB == 3 &&
                    raster.boundaries[2].provinceA == 2 &&
                    raster.boundaries[2].provinceB == 3,
                    "extração determinística de fronteiras");

    const auto previousPixels = raster.pixelProvinceIds;
    const std::vector<Pixel> unknownPixels = {
        {12, 34, 56}, {255, 0, 0}, {0, 255, 0},
        {0, 0, 255}, {0, 255, 0}, {0, 255, 0}
    };
    passed &= check(writeBmp(invalidBitmap, 3, 2, 24, unknownPixels),
                    "gerar BMP com cor desconhecida");
    passed &= check(!eulite::importProvinceRasterBmp(invalidBitmap.string(),
                                                     definitions.string(), raster, error),
                    "rejeitar cor RGB sem definição");
    passed &= check(raster.pixelProvinceIds == previousPixels,
                    "falha de importação preserva resultado anterior");

    const std::vector<Pixel> topDownPixels = {{255, 0, 0}, {0, 255, 0}};
    passed &= check(writeBmp(topDownBitmap, 2, -1, 32, topDownPixels),
                    "gerar BMP 32-bit top-down");
    eulite::ProvinceRasterData topDown;
    passed &= check(eulite::importProvinceRasterBmp(topDownBitmap.string(),
                                                    definitions.string(), topDown, error),
                    "importar BMP 32-bit top-down: " + error);
    passed &= check(topDown.width == 2 && topDown.height == 1 &&
                    topDown.pixelProvinceIds == std::vector<std::uint32_t>{1, 2},
                    "decodificar BMP 32-bit top-down");

    std::filesystem::remove_all(root, ec);
    if (!passed) return 1;
    std::cout << "Testes de importação de raster de províncias passaram.\n";
    return 0;
}
