#include "map/ProvinceContours.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool check(bool condition, const std::string& label) {
    if (!condition) std::cerr << "FALHOU: " << label << "\n";
    return condition;
}

const eulite::ProvincePolygon* findPolygon(
    const eulite::ProvinceContourData& data, std::uint32_t id) {
    const auto found = std::find_if(data.provinces.begin(), data.provinces.end(),
        [id](const eulite::ProvincePolygon& polygon) {
            return polygon.provinceId == id;
        });
    return found == data.provinces.end() ? nullptr : &*found;
}

} // namespace

int main() {
    bool passed = true;
    std::string error;

    eulite::ProvinceRasterData raster;
    raster.width = 3;
    raster.height = 2;
    raster.pixelProvinceIds = {1, 1, 2, 3, 2, 2};

    eulite::ProvinceContourData contours;
    passed &= check(eulite::extractProvinceContours(raster, contours, error),
                    "extrair contornos do raster retangular: " + error);
    passed &= check(contours.provinces.size() == 3 &&
                    contours.totalRingCount == 3,
                    "uma região conectada para cada província");
    passed &= check(contours.totalVertexCount == 14,
                    "simplificação remove vértices colineares");
    const auto* province1 = findPolygon(contours, 1);
    const auto* province2 = findPolygon(contours, 2);
    const auto* province3 = findPolygon(contours, 3);
    passed &= check(province1 && province1->rings.size() == 1 &&
                    province1->rings[0].points.size() == 4 &&
                    province1->rings[0].signedArea > 0.0 &&
                    !province1->rings[0].isHole,
                    "retângulo de província vira anel externo de quatro vértices");
    passed &= check(province2 && province2->rings.size() == 1 &&
                    province2->rings[0].points.size() == 6,
                    "província em L mantém seus seis cantos");
    passed &= check(province3 && province3->rings.size() == 1 &&
                    province3->rings[0].points.size() == 4,
                    "província de um pixel vira quadrilátero");

    // A province with a hole and a disconnected island of the same ID.
    eulite::ProvinceRasterData complexRaster;
    complexRaster.width = 5;
    complexRaster.height = 5;
    complexRaster.pixelProvinceIds.assign(25, 0);
    complexRaster.pixelProvinceIds[0] = 1;
    for (std::uint32_t y = 1; y <= 3; ++y) {
        for (std::uint32_t x = 1; x <= 3; ++x) {
            if (x == 2 && y == 2) continue;
            complexRaster.pixelProvinceIds[y * 5 + x] = 1;
        }
    }
    complexRaster.pixelProvinceIds[2 * 5 + 2] = 2;

    eulite::ProvinceContourData complexContours;
    passed &= check(eulite::extractProvinceContours(
                        complexRaster, complexContours, error),
                    "extrair contornos com buraco e ilha: " + error);
    const auto* complexProvince = findPolygon(complexContours, 1);
    const auto* centerProvince = findPolygon(complexContours, 2);
    passed &= check(complexProvince && complexProvince->rings.size() == 3,
                    "província desconectada gera anéis separados");
    if (complexProvince) {
        const auto holes = std::count_if(complexProvince->rings.begin(),
                                         complexProvince->rings.end(),
            [](const eulite::ProvinceContourRing& ring) { return ring.isHole; });
        passed &= check(holes == 1,
                        "buraco interno é identificado pela orientação do anel");
    }
    passed &= check(centerProvince && centerProvince->rings.size() == 1 &&
                    !centerProvince->rings[0].isHole,
                    "província dentro do buraco mantém seu próprio polígono");
    passed &= check(complexContours.totalRingCount == 4,
                    "contagem total de anéis inclui ilha e buraco");

    const auto svgPath = std::filesystem::temp_directory_path() /
                         "europa-lite-contours-test.svg";
    passed &= check(eulite::writeProvinceContoursSvg(
                        complexContours, 5, 5, svgPath.string(), error),
                    "exportar contornos para SVG: " + error);
    std::ifstream svgFile(svgPath);
    const std::string svg((std::istreambuf_iterator<char>(svgFile)),
                          std::istreambuf_iterator<char>());
    passed &= check(svg.find("<svg") != std::string::npos &&
                    svg.find("fill-rule=\"evenodd\"") != std::string::npos &&
                    svg.find("data-province-id=\"1\"") != std::string::npos,
                    "SVG contém polígonos e regra de preenchimento para buracos");
    std::error_code cleanupError;
    std::filesystem::remove(svgPath, cleanupError);

    // Invalid input must not overwrite the previous successful result.
    const auto previousProvinceCount = contours.provinces.size();
    raster.pixelProvinceIds.pop_back();
    passed &= check(!eulite::extractProvinceContours(raster, contours, error),
                    "rejeitar raster com dimensão inconsistente");
    passed &= check(contours.provinces.size() == previousProvinceCount,
                    "falha preserva a geometria previamente calculada");

    if (!passed) return 1;
    std::cout << "Testes de contornos e polígonos de províncias passaram.\n";
    return 0;
}
