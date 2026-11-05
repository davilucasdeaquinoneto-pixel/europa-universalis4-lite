#include "simulation/Game.hpp"
#include "map/MapPreview.hpp"
#include "map/ProvinceRaster.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

const char* connectionTypeName(eulite::ConnectionType type) {
    switch (type) {
    case eulite::ConnectionType::Land: return "terra";
    case eulite::ConnectionType::Sea: return "mar";
    case eulite::ConnectionType::Strait: return "estreito";
    case eulite::ConnectionType::River: return "rio";
    case eulite::ConnectionType::Impassable: return "intransponível";
    }
    return "desconhecido";
}

} // namespace

int main(int argc, char* argv[]) {
    const std::string dataDirectory = argc > 1 ? argv[1] : "data";

    eulite::Game game;
    std::string error;
    if (!game.start(dataDirectory, error)) {
        std::cerr << "Falha ao iniciar o mundo: " << error << "\n";
        std::cerr << "Verifique se a pasta data contém countries.csv e provinces.csv.\n";
        return 1;
    }

    std::cout << "Europa Universalis Lite - protótipo do motor de simulação\n";
    std::cout << "Digite 'help' para ver os comandos.\n";
    game.printStatus();

    std::string line;
    while (true) {
        std::cout << "europa> ";
        if (!std::getline(std::cin, line)) break;

        std::istringstream input(line);
        std::string command;
        input >> command;

        if (command.empty()) continue;
        if (command == "quit" || command == "exit") break;
        if (command == "help") {
            std::cout << "status          - mostra países e economia\n"
                      << "provinces       - lista as províncias\n"
                      << "geography       - mostra a hierarquia geográfica\n"
                      << "connections     - lista ligações entre províncias\n"
                      << "import-map BMP DEFINITIONS OUTPUT.csv - extrai fronteiras do raster\n"
                      << "tick [meses]    - avança o calendário (padrão: 1 mês)\n"
                      << "save [arquivo]  - salva o estado atual\n"
                      << "map [arquivo]   - gera um mapa isométrico SVG\n"
                      << "quit            - encerra o jogo\n";
        } else if (command == "status") {
            game.printStatus();
        } else if (command == "provinces") {
            game.printProvinces();
        } else if (command == "geography") {
            const auto& world = game.world();
            std::cout << "Continentes: " << world.continents().size()
                      << " | Regiões: " << world.regions().size()
                      << " | Sub-regiões: " << world.subregions().size()
                      << " | Áreas: " << world.areas().size()
                      << " | Províncias: " << world.provinces().size() << "\n";
            for (const auto& continent : world.continents()) {
                std::cout << "\n" << continent.name << "\n";
                for (const auto& region : world.regions()) {
                    if (region.continentId != continent.id) continue;
                    std::cout << "  " << region.name << "\n";
                    for (const auto& subregion : world.subregions()) {
                        if (subregion.regionId != region.id) continue;
                        std::cout << "    " << subregion.name << "\n";
                        for (const auto& area : world.areas()) {
                            if (area.subregionId == subregion.id) {
                                std::cout << "      " << area.name << "\n";
                            }
                        }
                    }
                }
            }
        } else if (command == "connections") {
            for (const auto& connection : game.world().connections()) {
                std::cout << connection.provinceA << " -> " << connection.provinceB
                          << " | " << connectionTypeName(connection.type)
                          << (connection.oneWay ? " | sentido único" : " | bidirecional")
                          << (game.world().hasPassableConnection(connection.provinceA,
                                                                  connection.provinceB)
                              ? " | transitável" : " | bloqueada") << "\n";
            }
        } else if (command == "import-map") {
            std::string bitmapPath, definitionPath, outputPath;
            if (!(input >> bitmapPath >> definitionPath >> outputPath)) {
                std::cout << "Uso: import-map provinces.bmp definition.csv adjacencies.csv\\n";
                continue;
            }

            eulite::ProvinceRasterData raster;
            if (!eulite::importProvinceRasterBmp(bitmapPath, definitionPath,
                                                  raster, error)) {
                std::cout << "Falha na importação: " << error << "\\n";
                continue;
            }

            std::ofstream adjacencyOutput(outputPath);
            if (!adjacencyOutput) {
                std::cout << "Não foi possível criar " << outputPath << "\\n";
                continue;
            }
            adjacencyOutput << "province_a,province_b\\n";
            for (const auto& boundary : raster.boundaries) {
                adjacencyOutput << boundary.provinceA << ','
                                << boundary.provinceB << "\\n";
            }
            std::cout << "Raster importado: " << raster.width << "x" << raster.height
                      << " | Províncias: " << raster.provincePixelCounts.size()
                      << " | Fronteiras: " << raster.boundaries.size()
                      << " | Arquivo: " << outputPath << "\\n";
        } else if (command == "tick") {
            int months = 1;
            if (input >> months && months < 0) {
                std::cout << "O número de meses não pode ser negativo.\n";
                continue;
            }
            game.advanceMonths(months);
            game.printStatus();
        } else if (command == "save") {
            std::string path = "save.eu4lite";
            input >> path;
            if (game.save(path, error)) {
                std::cout << "Jogo salvo em " << path << "\n";
            } else {
                std::cout << error << "\n";
            }
        } else if (command == "map") {
            std::string path = "mapa-demo.svg";
            input >> path;
            if (eulite::writeMapPreviewSvg(game.world(),
                                           dataDirectory + "/map_layout.csv",
                                           path, error)) {
                std::cout << "Mapa gerado em " << path << "\n";
            } else {
                std::cout << "Falha ao gerar mapa: " << error << "\n";
            }
        } else {
            std::cout << "Comando desconhecido. Digite 'help'.\n";
        }
    }

    std::cout << "Simulação encerrada.\n";
    return 0;
}
