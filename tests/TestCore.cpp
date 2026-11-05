#include "core/Date.hpp"
#include "world/World.hpp"

#include <cmath>
#include <iostream>
#include <string>

namespace {

bool check(bool condition, const std::string& message) {
    if (!condition) std::cerr << "FALHOU: " << message << "\n";
    return condition;
}

} // namespace

int main() {
    bool passed = true;

    eulite::GameDate date;
    passed &= check(date.toString() == "11/11/1444", "data inicial");
    date.advanceMonth();
    passed &= check(date.toString() == "11/12/1444", "avanço de novembro para dezembro");
    date.advanceMonth();
    passed &= check(date.toString() == "11/01/1445", "virada de ano");

    eulite::World world;
    std::string error;
    const std::string dataPath = std::string(EULITE_SOURCE_DIR) + "/data";
    passed &= check(world.load(dataPath, error), "carregamento do cenário: " + error);
    passed &= check(world.countries().size() == 4, "quantidade inicial de países");
    passed &= check(world.regions().size() == 3, "quantidade inicial de regiões");
    passed &= check(world.provinces().size() == 10, "quantidade inicial de províncias");
    passed &= check(world.findCountry("FRA") != nullptr, "busca de país por tag");
    passed &= check(world.findRegion(1) != nullptr, "busca de região por ID");
    passed &= check(world.findProvince(1) != nullptr, "busca de província por ID");
    passed &= check(world.countOwnedProvinces("CAS") == 3, "contagem de províncias de Castela");
    passed &= check(std::abs(world.developmentOf("FRA") - 21.0) < 0.0001,
                    "desenvolvimento total da França");
    passed &= check(world.areAdjacent(1, 2), "adjacência simétrica cadastrada");
    passed &= check(world.areAdjacent(2, 1), "adjacência no sentido inverso");
    passed &= check(!world.areAdjacent(1, 4), "províncias de regiões diferentes sem ligação");
    const auto* madrid = world.findProvince(1);
    passed &= check(madrid && madrid->adjacentProvinceIds.size() == 3,
                    "lista de vizinhos de Madrid incluindo ligação marítima");

    if (!passed) return 1;
    std::cout << "Todos os testes do núcleo passaram.\n";
    return 0;
}
