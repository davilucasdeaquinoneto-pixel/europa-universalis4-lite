#include "world/World.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace {

bool replaceText(const std::filesystem::path& path,
                 const std::string& from, const std::string& to) {
    std::ifstream input(path);
    if (!input) return false;
    std::string content((std::istreambuf_iterator<char>(input)),
                        std::istreambuf_iterator<char>());
    const auto position = content.find(from);
    if (position == std::string::npos) return false;
    content.replace(position, from.size(), to);
    std::ofstream output(path);
    output << content;
    return static_cast<bool>(output);
}

} // namespace

int main() {
    const std::filesystem::path source = EULITE_SOURCE_DIR;
    const std::filesystem::path data = source / "data";
    eulite::World world;
    std::string error;

    if (!world.load(data.string(), error)) {
        std::cerr << "FALHOU: carregar cenário geográfico: " << error << "\n";
        return 1;
    }

    bool passed = true;
    const auto check = [&passed](bool condition, const std::string& label) {
        if (!condition) {
            std::cerr << "FALHOU: " << label << "\n";
            passed = false;
        }
    };

    check(world.continents().size() == 1, "quantidade de continentes");
    check(world.regions().size() == 3, "quantidade de regiões");
    check(world.subregions().size() == 4, "quantidade de sub-regiões");
    check(world.areas().size() == 6, "quantidade de áreas");
    check(world.provinces().size() == 10, "quantidade de províncias");
    check(world.findContinent(1) && world.findContinent(1)->name == "Europa",
          "consulta de continente por ID");
    check(world.findRegion(1) && world.findRegion(1)->continentId == 1,
          "região vinculada ao continente");
    check(world.findSubregion(1) && world.findSubregion(1)->regionId == 1,
          "sub-região vinculada à região");
    check(world.findArea(1) && world.findArea(1)->subregionId == 1,
          "área vinculada à sub-região");
    check(world.findProvince(1) && world.findProvince(1)->areaId == 1,
          "província vinculada à área");

    check(world.areAdjacent(1, 2), "adjacência terrestre legada");
    check(world.areAdjacent(1, 7), "conexão marítima reconhecida como vizinhança");
    check(world.hasPassableConnection(1, 7) &&
          world.hasPassableConnection(7, 1), "conexão marítima bidirecional");
    check(world.hasPassableConnection(7, 8) &&
          world.hasPassableConnection(8, 7), "estreito bidirecional");
    check(world.hasPassableConnection(2, 5), "rio navegável no sentido definido");
    check(!world.hasPassableConnection(5, 2), "conexão de sentido único");
    check(world.areAdjacent(3, 4) && !world.hasPassableConnection(3, 4),
          "fronteira intransponível");

    const auto temporary = std::filesystem::temp_directory_path() /
                           "europa-lite-invalid-geography";
    std::error_code ec;
    std::filesystem::remove_all(temporary, ec);
    std::filesystem::create_directories(temporary);
    std::filesystem::copy(data, temporary,
        std::filesystem::copy_options::recursive |
        std::filesystem::copy_options::overwrite_existing, ec);
    check(!ec, "copiar cenário temporário");
    if (!ec) {
        check(replaceText(temporary / "geography.csv",
                          "area,1,Meseta Central,1",
                          "area,1,Meseta Central,999"),
              "preparar hierarquia inválida");
        const bool invalidLoaded = world.load(temporary.string(), error);
        check(!invalidLoaded, "rejeitar área com sub-região inexistente");
        check(world.continents().size() == 1 && world.provinces().size() == 10 &&
              world.findProvince(1) && world.findProvince(1)->areaId == 1,
              "falha transacional preserva o mundo carregado");
    }

    std::filesystem::remove_all(temporary, ec);
    if (!passed) return 1;
    std::cout << "Testes de hierarquia geográfica e conexões passaram.\n";
    return 0;
}
