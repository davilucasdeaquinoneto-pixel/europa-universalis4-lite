#include "map/MapPreview.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main() {
    const std::filesystem::path root =
        std::filesystem::temp_directory_path() / "europa-lite-map-preview-test";
    std::error_code ec;
    std::filesystem::remove_all(root, ec);
    std::filesystem::create_directories(root);

    eulite::World world;
    std::string error;
    const std::string sourceDir = std::string(EULITE_SOURCE_DIR);
    if (!world.load(sourceDir + "/data", error)) {
        std::cerr << "FALHOU: carregar mundo para mapa: " << error << "\n";
        return 1;
    }

    const auto outputPath = root / "preview.svg";
    if (!eulite::writeMapPreviewSvg(world, sourceDir + "/data/map_layout.csv",
                                    outputPath.string(), error)) {
        std::cerr << "FALHOU: gerar SVG: " << error << "\n";
        return 1;
    }

    std::ifstream file(outputPath);
    const std::string svg((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    const bool valid = svg.find("<svg") != std::string::npos &&
                       svg.find("<polygon") != std::string::npos &&
                       svg.find("Madrid") != std::string::npos &&
                       svg.find("não representa fronteiras históricas") != std::string::npos;
    std::filesystem::remove_all(root, ec);

    if (!valid) {
        std::cerr << "FALHOU: SVG não contém os elementos esperados.\n";
        return 1;
    }

    std::cout << "Teste do mapa isométrico passou.\n";
    return 0;
}
