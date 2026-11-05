#include "simulation/Game.hpp"
#include "map/MapPreview.hpp"

#include <iostream>
#include <sstream>
#include <string>

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
                      << "tick [meses]    - avança o calendário (padrão: 1 mês)\n"
                      << "save [arquivo]  - salva o estado atual\n"
                      << "map [arquivo]   - gera um mapa isométrico SVG\n"
                      << "quit            - encerra o jogo\n";
        } else if (command == "status") {
            game.printStatus();
        } else if (command == "provinces") {
            game.printProvinces();
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
